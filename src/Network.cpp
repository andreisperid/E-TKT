#include "Network.h"

#include <Arduino.h>
#include <AsyncElegantOTA.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <ESPmDNS.h>
#include <WiFi.h>

#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "Configuration.h"
#include "Display.h"
#include "ETKT.h"
#include "Logger.h"
#include "SPIFFS.h"
#include "esp_wifi.h"


Network *Network::instance = NULL;

Network::Network(Logger *logger, Display *display, ETKT *etkt,
                 uint8_t resetPin) {
  Network::instance = this;
  this->logger = logger;
  this->display = display;
  this->etkt = etkt;
  this->server = new AsyncWebServer(80);
  this->dns = new DNSServer();
  this->resetPin = resetPin;
}

Network::~Network() {
  delete server;
  delete dns;
}

void Network::softAPCallbackStatic(AsyncWiFiManager *manager) {
  if (instance) {
    instance->softAPCallback(manager);
  }
}

void Network::softAPCallback(AsyncWiFiManager *manager) {
  // captive portal to configure SSID and password
  this->display->renderConfig();
  this->logger->log(String("SoftAP SSID: ") + manager->getConfigPortalSSID());
}

void Network::clearWiFiCredentials() {
  // load the flash-saved configs
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);  // initiate and allocate wifi resources
  delay(2000);          // wait a bit

  // clear credentials if button is pressed
  if (esp_wifi_restore() != ESP_OK) {
    this->logger->log("WiFi is not initialized by esp_wifi_init ");
  } else {
    this->logger->log("WiFi Configurations Cleared!");
  }
  this->display->renderReset();
  delay(1500);
  esp_restart();
}

void Network::initialize() {
  pinMode(resetPin, INPUT);

  // local intialization. once its business is done, there is no need to keep it
  // around
  AsyncWiFiManager wifiManager(this->server, this->dns);

  // reset wifi settings if button is pressed
  if (!digitalRead(this->resetPin)) {
    this->clearWiFiCredentials();
  }
  // Set this->softAPCallback as wifiManager's APCallback.
  // This function will be called once the WiFiManager enters AP mode.
  wifiManager.setAPCallback(&Network::softAPCallbackStatic);
  wifiManager.setDebugOutput(DEBUG_WIFI);

  // fetches ssid and pass and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("E-TKT")) {
    this->logger->log("failed to connect and hit timeout");
    // reset and try again, or maybe put it to deep sleep
    ESP.restart();
  }

  if (!MDNS.begin("e-tkt")) {
    this->logger->log("Error starting mDNS");
  } else {
    // Advertise the webserver over mdns-sd, and add some custom props
    // to identify it as an e-tkt in case future integrations want to
    // find it.
    MDNS.addService("http", "tcp", 80);
    MDNS.addServiceTxt("http", "tcp", "e-tkt", "true");
  }

  // if you get here you have connected to the WiFi
  display->setConnectionInfo(WiFi.localIP().toString(), WiFi.SSID());

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    this->logger->log("An Error has occurred while mounting SPIFFS");
    return;
  }

  this->server->addHandler(new AsyncCallbackJsonWebHandler(
      "/api/cut",
      std::bind(&Network::cutPostHandler, this, std::placeholders::_1)));
  this->server->addHandler(new AsyncCallbackJsonWebHandler(
      "/api/feed",
      std::bind(&Network::feedPostHandler, this, std::placeholders::_1)));
  this->server->addHandler(new AsyncCallbackJsonWebHandler(
      "/api/reel",
      std::bind(&Network::reelPostHandler, this, std::placeholders::_1)));
  this->server->addHandler(new AsyncCallbackJsonWebHandler(
      "/api/save", std::bind(&Network::savePostHandler, this,
                             std::placeholders::_1, std::placeholders::_2)));
  this->server->addHandler(new AsyncCallbackJsonWebHandler(
      "/api/testalign",
      std::bind(&Network::testAlignPostHandler, this, std::placeholders::_1,
                std::placeholders::_2)));
  this->server->addHandler(new AsyncCallbackJsonWebHandler(
      "/api/testfull",
      std::bind(&Network::testFullPostHandler, this, std::placeholders::_1,
                std::placeholders::_2)));
  this->server->addHandler(new AsyncCallbackJsonWebHandler(
      "/api/home",
      std::bind(&Network::homePostHandler, this, std::placeholders::_1)));
  this->server->addHandler(new AsyncCallbackJsonWebHandler(
      "/api/move", std::bind(&Network::movePostHandler, this,
                             std::placeholders::_1, std::placeholders::_2)));
  this->server->addHandler(new AsyncCallbackJsonWebHandler(
      "/api/tag", std::bind(&Network::tagPostHandler, this,
                            std::placeholders::_1, std::placeholders::_2)));

  // Check printing status
  this->server->on(
      "/api/status", HTTP_GET,
      std::bind(&Network::statusGetHandler, this, std::placeholders::_1));

  // Serve static assets from the SPIFFS root directory.
  this->server->serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  // Handle 404s
  this->server->onNotFound(
      std::bind(&Network::notFoundHandler, this, std::placeholders::_1));

  if (ENABLE_OTA) {
    // Endpoint to accept OTA updates to hardware and firmware.
    AsyncElegantOTA.begin(server);
  }

  // Start server
  this->server->begin();
}

void Network::notFoundHandler(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void Network::savePostHandler(AsyncWebServerRequest *request,
                              JsonVariant &json) {
  const auto request_data = json.as<JsonObject>();
  auto response_data = new AsyncJsonResponse();
  const auto response_root = response_data->getRoot();
  try {
    if (!request_data.containsKey("align")) {
      response_root["error"] = "Please provide an align value";
      response_data->setCode(400);
    } else if (!request_data.containsKey("force")) {
      response_root["error"] = "Please provide a force value";
      response_data->setCode(400);
    } else {
      auto align = request_data["align"].as<int>();
      auto force = request_data["force"].as<int>();
      this->etkt->saveCommand(align, force);
      response_root["result"] = "success";
    }
  } catch (const std::exception &e) {
    response_root["error"] = e.what();
    response_data->setCode(400);
  }
  response_data->setLength();
  request->send(response_data);
}

void Network::homePostHandler(AsyncWebServerRequest *request) {
  auto response_data = new AsyncJsonResponse();
  const auto response_root = response_data->getRoot();
  try {
    this->etkt->homeCommand();
    response_root["result"] = "success";
  } catch (const std::exception &e) {
    response_root["error"] = e.what();
    response_data->setCode(400);
  }
  response_data->setLength();
  request->send(response_data);
}

void Network::movePostHandler(AsyncWebServerRequest *request,
                              JsonVariant &json) {
  const auto request_data = json.as<JsonObject>();
  auto response_data = new AsyncJsonResponse();
  const auto response_root = response_data->getRoot();
  try {
    this->etkt->moveCommand(request_data["character"].as<String>());
    response_root["result"] = "success";
  } catch (const std::exception &e) {
    response_root["error"] = e.what();
    response_data->setCode(400);
  }
  response_data->setLength();
  request->send(response_data);
}

void Network::tagPostHandler(AsyncWebServerRequest *request,
                             JsonVariant &json) {
  const auto request_data = json.as<JsonObject>();
  auto response_data = new AsyncJsonResponse();
  const auto response_root = response_data->getRoot();
  try {
    if (!request_data.containsKey("tag")) {
      response_root["error"] = "Please provide a tag value";
      response_data->setCode(400);
    } else {
      auto tag = request_data["tag"].as<String>();
      this->etkt->tagCommand(tag);
      response_root["result"] = "success";
    }
  } catch (const std::exception &e) {
    response_root["error"] = e.what();
    response_data->setCode(400);
  }
  response_data->setLength();
  request->send(response_data);
}

void Network::testAlignPostHandler(AsyncWebServerRequest *request,
                                   JsonVariant &json) {
  const auto request_data = json.as<JsonObject>();
  auto response_data = new AsyncJsonResponse();
  const auto response_root = response_data->getRoot();
  try {
    if (!request_data.containsKey("align")) {
      response_root["error"] = "Please provide an align value";
      response_data->setCode(400);
    } else {
      auto align = request_data["align"].as<int>();
      this->etkt->testAlignCommand(align);
      response_root["result"] = "success";
    }
  } catch (const std::exception &e) {
    response_root["error"] = e.what();
    response_data->setCode(400);
  }
  response_data->setLength();
  request->send(response_data);
}

void Network::testFullPostHandler(AsyncWebServerRequest *request,
                                  JsonVariant &json) {
  const auto request_data = json.as<JsonObject>();
  auto response_data = new AsyncJsonResponse();
  const auto response_root = response_data->getRoot();
  try {
    if (!request_data.containsKey("align")) {
      response_root["error"] = "Please provide an align value";
      response_data->setCode(400);
    } else if (!request_data.containsKey("force")) {
      response_root["error"] = "Please provide a force value";
      response_data->setCode(400);
    } else {
      auto align = request_data["align"].as<int>();
      auto force = request_data["force"].as<int>();
      this->etkt->testFullCommand(align, force);
      response_root["result"] = "success";
    }
  } catch (const std::exception &e) {
    response_root["error"] = e.what();
    response_data->setCode(400);
  }
  response_data->setLength();
  request->send(response_data);
}

void Network::cutPostHandler(AsyncWebServerRequest *request) {
  auto response_data = new AsyncJsonResponse();
  const auto response_root = response_data->getRoot();
  try {
    this->etkt->cutCommand();
    response_root["result"] = "success";
  } catch (const std::exception &e) {
    response_root["error"] = e.what();
    response_data->setCode(400);
  }
  response_data->setLength();
  request->send(response_data);
}

void Network::feedPostHandler(AsyncWebServerRequest *request) {
  auto response_data = new AsyncJsonResponse();
  const auto response_root = response_data->getRoot();
  try {
    this->etkt->feedCommand();
    response_root["result"] = "success";
  } catch (const std::exception &e) {
    response_root["error"] = e.what();
    response_data->setCode(400);
  }
  response_data->setLength();
  request->send(response_data);
}

void Network::reelPostHandler(AsyncWebServerRequest *request) {
  auto response_data = new AsyncJsonResponse();
  const auto response_root = response_data->getRoot();
  try {
    this->etkt->reelCommand();
    response_root["result"] = "success";
  } catch (const std::exception &e) {
    response_root["error"] = e.what();
    response_data->setCode(400);
  }
  response_data->setLength();
  request->send(response_data);
}

void Network::statusGetHandler(AsyncWebServerRequest *request) {
  AsyncJsonResponse *response = new AsyncJsonResponse();
  const JsonObject &root = response->getRoot();

  auto status = this->etkt->createStatus();
  root["progress"] = status->progress;
  root["busy"] = status->currentCommand != Command::IDLE;
  root["command"] = status->currentCommandString;
  root["align"] = status->align;
  root["force"] = status->force;

  // Return the current label, if relevant.
  if (status->currentCommand == Command::TAG) {
    root["current_label"] = status->currentLabel;
  }
  delete status;
  response->setLength();
  request->send(response);
}
