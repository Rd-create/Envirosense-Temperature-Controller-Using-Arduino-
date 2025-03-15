#include <WiFi.h>
#include <FirebaseESP32.h>
#include <DHT.h>

// WiFi Credentials
#define WIFI_SSID "//YOUR SSID"
#define WIFI_PASSWORD "YOUR PASSWORD"

// Firebase Credentials
#define FIREBASE_HOST "YOUR HOST ID "
#define FIREBASE_AUTH "YOUR PASS"

// DHT Sensor Config (Indoor & Outdoor)
#define DHTPIN_INDOOR 4    // Indoor DHT GPIO pin
#define DHTPIN_OUTDOOR 5   // Outdoor DHT GPIO pin
#define DHTTYPE DHT11      // DHT11 Sensor type

DHT dhtIndoor(DHTPIN_INDOOR, DHTTYPE);
DHT dhtOutdoor(DHTPIN_OUTDOOR, DHTTYPE);

FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    // Initialize DHT Sensors
    dhtIndoor.begin();
    dhtOutdoor.begin();
    Serial.println("🌡 Indoor & Outdoor DHT11 Sensors Initialized");

    // Connect to WiFi
    Serial.print("📶 Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\n✅ WiFi Connected!");
    
    // Firebase Initialization
    firebaseConfig.host = FIREBASE_HOST;
    firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&firebaseConfig, &firebaseAuth);
    Firebase.reconnectWiFi(true);
    Serial.println("🔥 Firebase Connected!");
}

void loop() {
    // Read Indoor Temperature
    float tempIndoor = dhtIndoor.readTemperature();
    
    // Read Outdoor Temperature
    float tempOutdoor = dhtOutdoor.readTemperature();

    // Check if readings are valid
    if (isnan(tempIndoor)) {
        Serial.println("❌ Error: Failed to read from Indoor DHT sensor! Retrying...");
        delay(2000);
        return;
    }
    
    if (isnan(tempOutdoor)) {
        Serial.println("❌ Error: Failed to read from Outdoor DHT sensor! Retrying...");
        delay(2000);
        return;
    }

    // Print Data to Serial Monitor
    Serial.print("🏠 Indoor Temperature: "); Serial.print(tempIndoor); Serial.println("°C");
    Serial.print("🌳 Outdoor Temperature: "); Serial.print(tempOutdoor); Serial.println("°C");

    // Upload Data to Firebase
    if (Firebase.setFloat(firebaseData, "/EnviroSense/Indoor/Temperature", tempIndoor)){
        Serial.println("✅ Indoor Temperature updated in Firebase");
    } else {
        Serial.println("❌ Failed to update Indoor Temperature");
        Serial.println(firebaseData.errorReason());
    }

    if (Firebase.setFloat(firebaseData, "/EnviroSense/Outdoor/Temperature", tempOutdoor)) {
        Serial.println("✅ Outdoor Temperature updated in Firebase");
    } else {
        Serial.println("❌ Failed to update Outdoor Temperature");
        Serial.println(firebaseData.errorReason());
    }

    delay(5000); // Wait 5 seconds before next update
}
