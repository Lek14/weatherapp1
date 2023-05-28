#include <iostream>
#include <string>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace std;
using namespace web;
using namespace web::http;
using namespace web::http::client;

// ����� ��� ������ � API ������
class WeatherApiClient {
private:
    http_client client; // HTTP-������

public:
    WeatherApiClient() : client(U("https://api.openweathermap.org/data/2.5")) {}

    // ����� ��� ��������� ������ � ������ �� �������� ������
    pplx::task<json::value> getWeatherData(const string& city, const string& apiKey) {
        uri_builder builder(U("/weather"));
        builder.append_query(U("q"), utility::conversions::to_string_t(city));
        builder.append_query(U("appid"), utility::conversions::to_string_t(apiKey));

        http_request request(methods::GET);
        request.set_request_uri(builder.to_string());

        return client.request(request).then([](http_response response) {
            return response.extract_json();
            });
    }
};

// ����� ��� ����������� � ��������� ������ ������
class WeatherApp {
private:
    WeatherApiClient weatherApiClient;

public:
    // ����� ��� ������� ����������
    void run() {
        setlocale(LC_ALL, "ru_RU.UTF-8");
        wcout << L"\t+++++++++++++++++++++++++++++++++" << endl;
        wcout << L"\t\t Weather App" << endl;
        wcout << L"\t+++++++++++++++++++++++++++++++++" << endl;
        wcout << L"\t������� �������� ������: ";
        wstring city;
        getline(wcin, city);

        weatherApiClient.getWeatherData(utility::conversions::to_utf8string(city), "93d0a2ff0b06808ac5c81f0563b1a5b1").then([this](json::value body) {
            try {
                double temperature = body[U("main")][U("temp")].as_double() - 273.15;  // Convert temperature from Kelvin to Celsius
                wcout << L"\t�����������: " << temperature << L"�C" << endl;
                wcout << L"\t���������: " << body[U("main")][U("humidity")].as_double() << L" % " << endl;
                utility::string_t weather = body[U("weather")][0][U("main")].as_string();
                wstring translatedWeather = getWeatherTranslation(weather);
                wcout << L"\t������: " << translatedWeather << endl;
            }
            catch (const json::json_exception& e) {
                wcout << L"������ ��� ��������� JSON-������: " << e.what() << endl;
            }
            }).wait();
    }

    // ����� ��� �������� �������� ������ �� ������� ����
    wstring getWeatherTranslation(const utility::string_t& weather) {
        if (weather == U("Clear")) {
            return L"����";
        }
        else if (weather == U("Clouds")) {
            return L"�������";
        }
        else if (weather == U("Rain")) {
            return L"�����";
        }
        else if (weather == U("Snow")) {
            return L"����";
        }
        else if (weather == U("Thunderstorm")) {
            return L"�����";
        }
        else {
            return utility::conversions::to_utf16string(weather);
        }
    }
};

int main() {
    WeatherApp weatherApp;
    weatherApp.run();

    return 0;
}
