#include <bits/stdc++.h>
#include <time.h>
using namespace std;

/*
** Handle errors
*/
vector<string> split(string input, char regex) {
    vector<string> tokens;
    string token;
    for (int i = 0; i < input.size(); i++) {
        if (input[i] != regex) {
            token += input[i];
        } else if (i > 0 && input[i - 1] != ' ') {
            tokens.push_back(token);
            token = "";
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

void writeToFile(string filename, string content) {
    // Kiểm tra file có tồn tại hay không
    ifstream ifile(filename.c_str());
    if (!ifile.fail()) {
        ifile.close();  // Đóng file
    } else {
        // Tạo file mới nếu file chưa tồn tại
        ofstream ofile(filename.c_str());
        ofile.close();
    }

    // Mở file để ghi
    ofstream ofile(filename.c_str(), std::ios::app);
    ofile << content << endl;
    ofile.close();
}

string getFileExtension(string filename) {
    return filename.substr(filename.find_last_of(".") + 1);
}

bool checkInputFile(string filename) {
    ifstream file(filename);
    bool check = file.good();
    file.close();
    return check;
}

bool checkOutputFile(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        ofstream ofile(filename.c_str());
        ofile.close();
        return true;
    }
    bool check = file.good();
    file.close();
    return check;
}

bool checkCommand(const string& command) {
    vector<string> tokens = split(command, ' ');
    if (tokens.size() < 3) {
        cout << "Error 01: invalid command";
        writeToFile("dust_convert_error.log", "Error 01: invalid command");
        return false;
    }
    if (tokens[0] != "dust_convert") {
        cout << "Error 01: invalid command";
        writeToFile("dust_convert_error.log", "Error 01: invalid command");
        return false;
    }
    if ((getFileExtension(tokens[1]) != "dat" &&
         getFileExtension(tokens[1]) != "csv") ||
        (getFileExtension(tokens[2]) != "dat" &&
         getFileExtension(tokens[2]) != "csv")) {
        cout << "Error 03: invalid file format";
        writeToFile("dust_convert_error.log", "Error 03: invalid file format");
        return false;
    }
    if ((getFileExtension(tokens[1]) == "dat" &&
         getFileExtension(tokens[2]) == "dat") ||
        (getFileExtension(tokens[1]) == "csv" &&
         getFileExtension(tokens[2]) == "csv")) {
        cout << "Error 01: invalid command";
        writeToFile("dust_convert_error.log", "Error 03: invalid command");
        return false;
    }
    if (!checkInputFile(tokens[1])) {
        string error02Message = "Error 02: denied access ";
        cout << error02Message << tokens[1];
        writeToFile("dust_convert_error.log", error02Message.append(tokens[1]));
        return false;
    }
    if (!checkOutputFile(tokens[2])) {
        string error02Message = "Error 02: denied access ";
        cout << error02Message << tokens[2];
        writeToFile("dust_convert_error.log", error02Message.append(tokens[2]));
        return false;
    }
    if (tokens.size() > 3) {
        if (tokens.size() == 6) {
            if (tokens[3] != "-s") {
                cout << "Error 01: invalid command";
                writeToFile("dust_convert_error.log",
                            "Error 01: invalid command");
                return false;
            }
            vector<string> fields = split(tokens[4], ',');
            for (int i = 0; i < fields.size(); i++) {
                if (!(fields[i] == "id" || fields[i] == "time" ||
                      fields[i] == "values")) {
                    cout << "Error 01: invalid command";
                    writeToFile("dust_convert_error.log",
                                "Error 01: invalid command");
                    return false;
                    break;
                }
            }
            for (int i = 0; i < fields.size() - 1; i++) {
                for (int j = i + 1; j < fields.size(); j++) {
                    if (fields[i] == fields[j]) {
                        cout << "Error 01: invalid command";
                        writeToFile("dust_convert_error.log",
                                    "Error 01: invalid command");
                        return false;
                        break;
                    }
                }
            }
            if (tokens[5] != "-asc" && tokens[5] != "-desc") {
                cout << "Error 01: invalid command";
                writeToFile("dust_convert_error.log",
                            "Error 01: invalid command");
                return false;
            }
        } else if (tokens.size() == 5) {
            if (tokens[3] != "-s") {
                cout << "Error 01: invalid command";
                writeToFile("dust_convert_error.log",
                            "Error 01: invalid command");
                return false;
            }
            if (tokens[5] != "-asc" && tokens[5] != "-desc") {
                cout << "Error 01: invalid command";
                writeToFile("dust_convert_error.log",
                            "Error 01: invalid command");
                return false;
            }
        } else {
            cout << "Error 01: invalid command";
            writeToFile("dust_convert_error.log", "Error 01: invalid command");
            return false;
        }
    }
    return true;
}

/*
** Convert: from .csv to .dat
*/

string calculateChecksum(const string& packetLength, const string& id,
                         const string& time, const string& pm25,
                         const string& aqi) {
    // cout << packetLength << " " << id << " " << time << " " << pm25 << " "
    //      << aqi << endl;
    // Tính tổng các byte
    unsigned int sum = stoi(packetLength, nullptr, 16) + stoi(id, nullptr, 16) +
                       stoi(time.substr(0, 2), nullptr, 16) +
                       stoi(time.substr(2, 2), nullptr, 16) +
                       stoi(time.substr(4, 2), nullptr, 16) +
                       stoi(time.substr(6, 2), nullptr, 16) +
                       stoi(pm25.substr(0, 2), nullptr, 16) +
                       stoi(pm25.substr(2, 2), nullptr, 16) +
                       stoi(pm25.substr(4, 2), nullptr, 16) +
                       stoi(pm25.substr(6, 2), nullptr, 16) +
                       stoi(aqi.substr(0, 2), nullptr, 16) +
                       stoi(aqi.substr(2, 2), nullptr, 16);
    // Lấy phần dư khi chia cho 256
    unsigned char remainder = sum % 0x100;
    // Tính phần bù 2 của phần dư
    unsigned char checksum = 0xff - remainder + 1;
    stringstream ss;
    ss << hex << static_cast<int>(checksum);
    string checksumHex = ss.str();
    return checksumHex;
}

vector<string> readFile(string file_name) {
    vector<string> lines;
    ifstream file(file_name);
    string line;

    while (file && !file.eof()) {
        getline(file, line);
        lines.push_back(line);
    }
    file.close();
    for (int i = 0; i < lines.size(); i++) {
        if (lines[i] == "") {
            lines.erase(lines.begin() + i);
        }
    }
    return lines;
}

string formatHex(string str) {
    string result;
    for (int i = 0; i < str.length(); i++) {
        if (i % 2 == 1 && i < str.length() - 1) {
            result += tolower(str[i]);
            result += ' ';
        } else {
            result += tolower(str[i]);
        }
    }
    return result;
}

// Check valid line in file csv
bool isPositiveInteger(string s) {
    if (s.empty()) {
        return false;
    }
    for (char c : s) {
        if (!isdigit(c)) {
            return false;
        }
    }
    int num = stoi(s);
    if (num <= 0) {
        return false;
    }
    return true;
}

bool isValidDateTimeFormat(string s) {
    if (s.length() != 19) {
        return false;
    }
    for (int i = 0; i < s.length(); i++) {
        if (i == 4 || i == 7) {
            if (s[i] != ':') {
                return false;
            }
        } else if (i == 10) {
            if (s[i] != ' ') {
                return false;
            }
        } else if (i == 13 || i == 16) {
            if (s[i] != ':') {
                return false;
            }
        } else if (s[i] < '0' || s[i] > '9') {
            return false;
        }
    }
    struct tm timeinfo = {0};
    timeinfo.tm_year = stoi(s.substr(0, 4)) - 1900;
    timeinfo.tm_mon = stoi(s.substr(5, 2)) - 1;
    timeinfo.tm_mday = stoi(s.substr(8, 2));
    timeinfo.tm_hour = stoi(s.substr(11, 2));
    timeinfo.tm_min = stoi(s.substr(14, 2));
    timeinfo.tm_sec = stoi(s.substr(17, 2));
    if (mktime(&timeinfo) == -1) {
        return false;
    }
    return true;
}

bool isValidValue(string s) {
    try {
        double number = stod(s);
        if (number > 0.0 && number <= 550.5) {
            return true;
        }
    } catch (const std::invalid_argument& ia) {
        return false;
    }
    return false;
}

bool isValidLineCsv(string line) {
    vector<string> items = split(line, ',');
    if (items.size() != 5) {
        return false;
    }
    string id = items[0];
    if (!isPositiveInteger(id)) {
        return false;
    }
    string time = items[1];
    if (!isValidDateTimeFormat(time)) {
        return false;
    }
    string value = items[2];
    if (!isValidValue(value)) {
        return false;
    }
    string aqi = items[3];
    if (!isPositiveInteger(aqi) ||
        (isPositiveInteger(aqi) && stoi(aqi) > 500)) {
        return false;
    }
    string pollution = items[4];
    if (pollution != "Good" && pollution != "Moderate" &&
        pollution != "Slightly unhealthy" && pollution != "Unhealthy" &&
        pollution != "Very unhealthy" && pollution != "Hazardous" &&
        pollution != "Extremely hazardous") {
        return false;
    }
    return true;
}

bool isErrorPosition(vector<int> errorPositions, int index) {
    for (int i = 0; i < errorPositions.size(); i++) {
        if (index == errorPositions[i]) {
            return true;
            break;
        }
    }
    return false;
}

void encode(string inputFilename, string outputFilename) {
    vector<string> lines = readFile(inputFilename);
    vector<int> errorPositions;
    for (int i = 1; i < lines.size(); i++) {
        if (!isValidLineCsv(lines[i])) {
            writeToFile(
                "dust_convert_error.log",
                "Error 04: data is missing at line " + to_string(i + 1));
            errorPositions.push_back(i - 1);
        }
    }
    for (int i = 1; i < lines.size() - 1; i++) {
        for (int j = i + 1; j < lines.size(); j++) {
            if (!isErrorPosition(errorPositions, i - 1) &&
                !isErrorPosition(errorPositions, j - 1) &&
                lines[i] == lines[j]) {
                errorPositions.push_back(j - 1);
                writeToFile("dust_convert_error.log",
                            "Error 05: duplicated data at lines " +
                                to_string(i + 1) + ", " + to_string(j + 1));
            }
        }
    }
    int numLines = lines.size() - 1;
    int numErrorLines = errorPositions.size();
    int numValidLines = numLines - numErrorLines;
    writeToFile("dust_convert_run.log",
                "Total number of rows: " + to_string(numLines));
    writeToFile("dust_convert_run.log",
                "Succesfully converted rows: " + to_string(numValidLines));
    writeToFile("dust_convert_run.log",
                "Error rows: " + to_string(numErrorLines));
    vector<string> results;
    for (int i = 1; i < lines.size(); i++) {
        vector<string> items = split(lines[i], ',');
        if (isErrorPosition(errorPositions, i - 1)) {
            continue;
        }
        for (int j = 0; j < items.size(); j++) {
            results.push_back(items[j]);
        }
    }
    vector<string> ids;
    vector<string> seconds;
    vector<string> values;
    vector<string> aqis;
    vector<string> checksums;
    for (int i = 0; i < results.size(); i++) {
        if (i % 5 == 0) {
            int id = stod(results[i]);
            stringstream ss;
            ss << hex << id;
            string hex_id = ss.str();
            if (hex_id.length() < 2) {
                hex_id = "0" + hex_id;
            }
            ids.push_back(hex_id);
        } else if (i % 5 == 1) {
            tm tm = {};
            istringstream ss(results[i]);
            ss >> get_time(&tm, "%Y:%m:%d %H:%M:%S");
            time_t second = mktime(&tm);
            stringstream stream;
            stream << hex << second;
            string hex_second = stream.str();
            if (hex_second.length() < 8) {
                int len = hex_second.length();
                for (int j = 0; j < (8 - len); j++) {
                    hex_second = "0" + hex_second;
                }
            }
            seconds.push_back(hex_second);
        } else if (i % 5 == 2) {
            float value = stof(results[i]);
            unsigned int i;
            memcpy(&i, &value, sizeof(value));

            bitset<32> b(i);
            stringstream stream;
            stream << hex << b.to_ulong();
            string hex_value(stream.str());
            if (hex_value.length() < 8) {
                int len = hex_value.length();
                for (int j = 0; j < (8 - len); j++) {
                    hex_value = "0" + hex_value;
                }
            }
            values.push_back(hex_value);
        } else if (i % 5 == 3) {
            int aqi = stod(results[i]);
            stringstream ss;
            ss << hex << aqi;
            string hex_aqi = ss.str();
            if (hex_aqi.length() < 4) {
                int len = hex_aqi.length();
                for (int j = 0; j < (4 - len); j++) {
                    hex_aqi = "0" + hex_aqi;
                }
            }
            aqis.push_back(hex_aqi);
        }
    }
    // vector<string> encoded_lines;
    ofstream of(outputFilename);
    for (int i = 0; i < ids.size(); i++) {
        string checksum =
            calculateChecksum("0f", ids[i], seconds[i], values[i], aqis[i]);
        if (checksum.length() < 2) {
            checksum = '0' + checksum;
        }
        string line = "00 0f " + ids[i] + " " + formatHex(seconds[i]) + " " +
                      formatHex(values[i]) + " " + formatHex(aqis[i]) + " " +
                      checksum + " ff";
        of << line << endl;
    }
    of.close();
}

/*
** Sorting algorithm
*/
// Bubble sort with swap check
vector<int> bubbleSort(vector<vector<float>> values, string type) {
    vector<pair<int, vector<float>>> pairs;
    for (int i = 0; i < values.size(); i++) {
        pairs.push_back(make_pair(i, values[i]));
    }
    int numFields = values[0].size();
    if (type == "-asc") {
        auto start = chrono::high_resolution_clock::now();
        bool swapped;
        for (int i = 0; i < pairs.size() - 1; i++) {
            swapped = false;
            for (int j = 0; j < pairs.size() - i - 1; j++) {
                int k = 0;
                while (pairs[j].second[k] == pairs[j + 1].second[k] &&
                       k < numFields - 1) {
                    k++;
                }
                if (pairs[j].second[k] > pairs[j + 1].second[k]) {
                    swap(pairs[j], pairs[j + 1]);
                    swapped = true;
                }
            }
            if (!swapped) {
                break;
            }
        }
        auto end = chrono::high_resolution_clock::now();
        auto diff = end - start;
        double time = chrono::duration<double, milli>(diff).count();
        writeToFile("dust_convert_run.log",
                    "Sorting algorithm bubble with swap check [ms]: " +
                        to_string(time));
    } else if (type == "-desc") {
        auto start = chrono::high_resolution_clock::now();
        bool swapped;
        for (int i = 0; i < pairs.size() - 1; i++) {
            swapped = false;
            for (int j = 0; j < pairs.size() - i - 1; j++) {
                int k = 0;
                while (pairs[j].second[k] == pairs[j + 1].second[k] &&
                       k < numFields - 1) {
                    k++;
                }
                if (pairs[j].second[k] < pairs[j + 1].second[k]) {
                    swap(pairs[j], pairs[j + 1]);
                    swapped = true;
                }
            }
            if (!swapped) {
                break;
            }
        }
        auto end = chrono::high_resolution_clock::now();
        auto diff = end - start;
        double time = chrono::duration<double, milli>(diff).count();
        writeToFile("dust_convert_run.log",
                    "Sorting algorithm bubble with swap check [ms]: " +
                        to_string(time));
    }
    vector<int> sortedPositions;
    for (int i = 0; i < pairs.size(); i++) {
        sortedPositions.push_back(pairs[i].first);
    }
    return sortedPositions;
}

// Insertion sort
bool isGreaterThan(vector<float> a, vector<float> b) {
    int k = 0;
    while (k < a.size() - 1 && a[k] == b[k]) {
        k++;
    }
    return a[k] > b[k];
}

bool isLessThan(vector<float> a, vector<float> b) {
    int k = 0;
    while (k < a.size() - 1 && a[k] == b[k]) {
        k++;
    }
    return a[k] < b[k];
}

vector<int> insertionSort(vector<vector<float>> values, string type) {
    vector<pair<int, vector<float>>> pairs;
    for (int i = 0; i < values.size(); i++) {
        pairs.push_back(make_pair(i, values[i]));
    }
    int numFields = values[0].size();
    if (type == "-asc") {
        auto start = chrono::high_resolution_clock::now();
        for (int i = 1; i < pairs.size(); i++) {
            pair<int, vector<float>> keyPair = pairs[i];
            int j = i - 1;
            while (j >= 0 && isLessThan(keyPair.second, pairs[j].second)) {
                pairs[j + 1] = pairs[j];
                j--;
            }
            pairs[j + 1] = keyPair;
        }
        auto end = chrono::high_resolution_clock::now();
        auto diff = end - start;
        double time = chrono::duration<double, milli>(diff).count();
        writeToFile("dust_convert_run.log",
                    "Sorting algorithm insertion[ms]: " + to_string(time));
    } else if (type == "-desc") {
        auto start = chrono::high_resolution_clock::now();
        for (int i = 1; i < pairs.size(); i++) {
            pair<int, vector<float>> keyPair = pairs[i];
            int j = i - 1;
            while (j >= 0 && isGreaterThan(keyPair.second, pairs[j].second)) {
                pairs[j + 1] = pairs[j];
                j--;
            }
            pairs[j + 1] = keyPair;
        }
        auto end = chrono::high_resolution_clock::now();
        auto diff = end - start;
        double time = chrono::duration<double, milli>(diff).count();
        writeToFile("dust_convert_run.log",
                    "Sorting algorithm insertion[ms]: " + to_string(time));
    }
    vector<int> sortedPositions;
    for (int i = 0; i < pairs.size(); i++) {
        sortedPositions.push_back(pairs[i].first);
    }
    return sortedPositions;
}

/*
** Convert: from .dat to .csv
*/
float hexValueToFloat(string hexString) {
    unsigned int hexInt;
    sscanf(hexString.c_str(), "%x", &hexInt);
    float result;
    memcpy(&result, &hexInt, sizeof(result));
    return round(result * pow(10, 1)) / pow(10, 1);
}

string removeTrailingZeros(string str) {
    int len = str.length();
    int i = len - 1;
    while (str[i] == '0' && str[i - 1] != '.') {
        str.erase(i, 1);
        i--;
    }
    return str;
}

float hexAQIToFloat(string hexString) {
    // Convert the hex string to an unsigned integer
    unsigned int hexInt = stoi(hexString, 0, 16);

    // Convert the integer to binary string
    bitset<16> bits(hexInt);
    string binaryString = bits.to_string();

    // Extract the sign bit, exponent and mantissa from the binary string
    int sign = binaryString[0] - '0';
    int exponent = stoi(binaryString.substr(1, 8), 0, 2) - 127;
    int mantissa = stoi(binaryString.substr(9, 7), 0, 2);

    // Calculate the value of the float based on the sign, exponent and mantissa
    float result =
        pow(-1, sign) * (1 + (float)mantissa / pow(2, 7)) * pow(2, exponent);

    return result;
}

// Check error file .dat
bool isValidHexId(const string& hexString) {
    if (hexString.size() != 2) {
        return false;
    }
    for (char c : hexString) {
        if (!isxdigit(c)) {
            return false;
        }
    }
    int num = stoi(hexString, nullptr, 16);
    return num > 0 && num <= 255;
}

bool isValidHexTime(const string& hexTime) {
    if (hexTime.size() != 8) {
        return false;
    }
    for (char c : hexTime) {
        if (!isxdigit(c)) {
            return false;
        }
    }
    stringstream ss;
    ss << hex << hexTime;
    uint32_t time_stamp;
    ss >> time_stamp;
    auto now = chrono::system_clock::now().time_since_epoch();
    auto seconds = chrono::duration_cast<chrono::seconds>(now).count();
    return time_stamp < seconds;
}

bool isValidHexValue(const string& hexValue) {
    if (hexValue.size() != 8) {
        return false;
    }
    for (char c : hexValue) {
        if (!isxdigit(c)) {
            return false;
        }
    }
    unsigned int hexInt;
    sscanf(hexValue.c_str(), "%x", &hexInt);
    float value;
    memcpy(&value, &hexInt, sizeof(value));

    return (value > 0) && (value <= 550.5);
}

bool isValidHexAQI(const string& hexAQI) {
    if (hexAQI.size() != 4) {
        return false;
    }
    for (char c : hexAQI) {
        if (!isxdigit(c)) {
            return false;
        }
    }
    stringstream ss;
    ss << hex << hexAQI;
    int value;
    ss >> value;
    return (value > 0) && (value <= 500);
}

float valueToAQI(float value, float in_min, float in_max, float out_min,
                 float out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int calculateAQIFromValue(float value) {
    if (value > 0 && value < 12) {
        return round(valueToAQI(value, 0, 12, 0, 50));
    } else if (value >= 12 && value < 35.5) {
        return round(valueToAQI(value, 12, 35.5, 50, 100));
    } else if (value >= 35.5 && value < 55.5) {
        return round(valueToAQI(value, 35.5, 55.5, 100, 150));
    } else if (value >= 55.5 && value < 150.5) {
        return round(valueToAQI(value, 55.5, 150.5, 150, 200));
    } else if (value >= 150.5 && value < 250.5) {
        return round(valueToAQI(value, 150.5, 250.5, 200, 300));
    } else if (value >= 250.5 && value < 350.5) {
        return round(valueToAQI(value, 250.5, 350.5, 300, 400));
    } else if (value >= 350.5 && value <= 550.5) {
        return round(valueToAQI(value, 350.5, 550.5, 400, 500));
    } else {
        return -1;
    }
}

bool isValidLineHex(string line) {
    vector<string> items = split(line, ' ');
    if (items.size() != 15) {
        return false;
    }
    string startByte = items[0];
    if (startByte != "00") {
        return false;
    }
    string packetLength = items[1];
    if (packetLength != "0F" && packetLength != "0f") {
        return false;
    }
    string id = items[2];
    if (!isValidHexId(id)) {
        return false;
    }
    string time = items[3] + items[4] + items[5] + items[6];
    if (!isValidHexTime(time)) {
        return false;
    }
    string value = items[7] + items[8] + items[9] + items[10];
    if (!isValidHexValue(value)) {
        return false;
    }
    string aqi = items[11] + items[12];
    if (!isValidHexAQI(aqi)) {
        return false;
    }
    string checkSum = items[13];
    string correctCheckSum =
        calculateChecksum(packetLength, id, time, value, aqi);
    for (char c : checkSum) {
        c = toupper(c);
    }
    for (char c : correctCheckSum) {
        c = toupper(c);
    }
    if (checkSum != correctCheckSum) {
        return false;
    }
    int correctAQI = calculateAQIFromValue(hexValueToFloat(value));
    int aqi_value = stoi(aqi, nullptr, 16);
    if (correctAQI != aqi_value) {
        return false;
    }
    string stopByte = items[14];
    if (stopByte != "ff" && stopByte != "FF" && stopByte != "fF" &&
        stopByte != "Ff") {
        return false;
    }
    return true;
}

void decode(string inputFilename, string outputFilename, vector<string> sortBy,
            string order) {
    vector<string> lines = readFile(inputFilename);
    vector<int> errorPositions;
    for (int i = 0; i < lines.size(); i++) {
        if (!isValidLineHex(lines[i])) {
            writeToFile(
                "dust_convert_error.log",
                "Error 06: invalid data packet at line " + to_string(i + 1));
            errorPositions.push_back(i);
        }
    }
    for (int i = 0; i < lines.size() - 1; i++) {
        for (int j = i + 1; j < lines.size(); j++) {
            if (!isErrorPosition(errorPositions, i) &&
                !isErrorPosition(errorPositions, j) && lines[i] == lines[j]) {
                errorPositions.push_back(j);
                writeToFile("dust_convert_error.log",
                            "Error 05: duplicated data at lines " +
                                to_string(i + 1) + ", " + to_string(j + 1));
            }
        }
    }
    int numLines = lines.size() - 1;
    int numErrorLines = errorPositions.size();
    int numValidLines = numLines - numErrorLines;
    writeToFile("dust_convert_run.log",
                "Total number of rows: " + to_string(numLines));
    writeToFile("dust_convert_run.log",
                "Succesfully converted rows: " + to_string(numValidLines));
    writeToFile("dust_convert_run.log",
                "Error rows: " + to_string(numErrorLines));
    vector<string> results;
    for (int i = 0; i < lines.size(); i++) {
        vector<string> items = split(lines[i], ' ');
        if (isErrorPosition(errorPositions, i)) {
            continue;
        }
        results.push_back(items[0]);  // start byte
        results.push_back(items[1]);  // packet length
        results.push_back(items[2]);  // id
        results.push_back(items[3] + items[4] + items[5] + items[6]);   // time
        results.push_back(items[7] + items[8] + items[9] + items[10]);  // value
        results.push_back(items[11] + items[12]);                       // aqi
        results.push_back(items[13]);  // checksum
        results.push_back(items[14]);  // stop byte
    }
    vector<float> ids;
    vector<string> times;
    vector<float> seconds;
    vector<float> values;
    vector<int> aqis;
    vector<string> pollutions;
    for (int i = 0; i < results.size(); i++) {
        if (i % 8 == 2) {
            int id_value = stoi(results[i], nullptr, 16);
            ids.push_back(id_value);
        } else if (i % 8 == 3) {
            unsigned int hex_val;
            stringstream ss;
            ss << hex << results[i];
            ss >> hex_val;

            time_t time = static_cast<time_t>(hex_val);
            struct tm* time_info = gmtime(&time);

            char buffer[20];
            strftime(buffer, 20, "%Y:%m:%d %H:%M:%S", time_info);
            string time_str(buffer);
            times.push_back(time_str);

            // Get seconds
            tm time_tm = {};
            istringstream iss(time_str);
            string token;
            getline(iss, token, ':');
            time_tm.tm_year = stoi(token) - 1900;
            getline(iss, token, ':');
            time_tm.tm_mon = stoi(token) - 1;
            getline(iss, token, ' ');
            time_tm.tm_mday = stoi(token);
            getline(iss, token, ':');
            time_tm.tm_hour = stoi(token);
            getline(iss, token, ':');
            time_tm.tm_min = stoi(token);
            getline(iss, token);
            time_tm.tm_sec = stoi(token);
            chrono::system_clock::time_point tp =
                chrono::system_clock::from_time_t(mktime(&time_tm));
            chrono::duration<int, ratio<1>> duration =
                chrono::duration_cast<chrono::duration<int, ratio<1>>>(
                    tp.time_since_epoch());
            int sec = duration.count();
            seconds.push_back(sec);
        } else if (i % 8 == 4) {
            float value = hexValueToFloat(results[i]);
            values.push_back(value);
        } else if (i % 8 == 5) {
            int aqi_value = stoi(results[i], nullptr, 16);
            aqis.push_back(aqi_value);
            if (aqi_value > 0 && aqi_value <= 50) {
                pollutions.push_back("Good");
            } else if (aqi_value > 50 && aqi_value <= 100) {
                pollutions.push_back("Moderate");
            } else if (aqi_value > 100 && aqi_value <= 150) {
                pollutions.push_back("Slightly unhealthy");
            } else if (aqi_value > 150 && aqi_value <= 200) {
                pollutions.push_back("Unhealthy");
            } else if (aqi_value > 200 && aqi_value <= 300) {
                pollutions.push_back("Very unhealthy");
            } else if (aqi_value > 300 && aqi_value <= 400) {
                pollutions.push_back("Hazardous");
            } else if (aqi_value >= 400 && aqi_value <= 500) {
                pollutions.push_back("Extremely hazardous");
            }
        }
    }
    ofstream of(outputFilename);
    of << "id,time,values,aqi,pollution" << endl;
    vector<int> sortedPositions;
    vector<vector<float>> fields;
    if (sortBy.size() > 0) {
        for (int i = 0; i < ids.size(); i++) {
            vector<float> field = {0};
            fields.push_back(field);
        }
        if (sortBy[0] == "id") {
            for (int i = 0; i < ids.size(); i++) {
                fields[i][0] = ids[i];
            }
        } else if (sortBy[0] == "time") {
            for (int i = 0; i < ids.size(); i++) {
                fields[i][0] = seconds[i];
            }
        } else if (sortBy[0] == "values") {
            for (int i = 0; i < ids.size(); i++) {
                fields[i][0] = values[i];
            }
        }
        if (sortBy.size() >= 2) {
            if (sortBy[1] == "id") {
                for (int i = 0; i < ids.size(); i++) {
                    fields[i].push_back(ids[i]);
                }
            } else if (sortBy[1] == "time") {
                for (int i = 0; i < ids.size(); i++) {
                    fields[i].push_back(seconds[i]);
                }
            } else if (sortBy[1] == "values") {
                for (int i = 0; i < ids.size(); i++) {
                    fields[i].push_back(values[i]);
                }
            }
        }
        if (sortBy.size() >= 3) {
            if (sortBy[2] == "id") {
                for (int i = 0; i < ids.size(); i++) {
                    fields[i].push_back(ids[i]);
                }
            } else if (sortBy[2] == "time") {
                for (int i = 0; i < ids.size(); i++) {
                    fields[i].push_back(seconds[i]);
                }
            } else if (sortBy[2] == "values") {
                for (int i = 0; i < ids.size(); i++) {
                    fields[i].push_back(values[i]);
                }
            }
        }
    }
    if (sortBy.size() > 0 && order == "-asc") {
        sortedPositions = bubbleSort(fields, "-asc");
        sortedPositions = insertionSort(fields, "-asc");
    } else if (sortBy.size() > 0 && order == "-desc") {
        sortedPositions = bubbleSort(fields, "-desc");
        sortedPositions = insertionSort(fields, "-desc");
    }
    if (sortBy.size() < 1) {
        for (int i = 0; i < ids.size(); i++) {
            stringstream ss;
            ss << values[i];
            string strValue = ss.str();
            if (isPositiveInteger(strValue)) {
                strValue += ".0";
            }
            string line = to_string((int)ids[i]) + "," + times[i] + "," +
                          strValue + "," + to_string((int)aqis[i]) + "," +
                          pollutions[i];
            of << line << endl;
        }
        of.close();
    } else {
        for (int i = 0; i < ids.size(); i++) {
            stringstream ss;
            ss << values[sortedPositions[i]];
            string strValue = ss.str();
            if (isPositiveInteger(strValue)) {
                strValue += ".0";
            }
            string line = to_string((int)ids[sortedPositions[i]]) + "," +
                          times[sortedPositions[i]] + "," + strValue + "," +
                          to_string(aqis[sortedPositions[i]]) + "," +
                          pollutions[sortedPositions[i]];
            of << line << endl;
        }
        of.close();
    }
}

int main() {
    string command;
    cout << "Enter command: ";
    getline(cin, command);
    if (!checkCommand(command)) {
        exit(0);
    }
    vector<string> tokens = split(command, ' ');
    if (getFileExtension(tokens[1]) == "csv" &&
        getFileExtension(tokens[2]) == "dat") {
        encode(tokens[1], tokens[2]);
    } else {
        if (tokens.size() == 3) {
            vector<string> v;
            decode(tokens[1], tokens[2], v, "");
        }
        if (tokens.size() == 5) {
            vector<string> v = {"id", "time", "values"};
            decode(tokens[1], tokens[2], v, tokens[4]);
        }
        if (tokens.size() == 6) {
            vector<string> fields = split(tokens[4], ',');
            decode(tokens[1], tokens[2], fields, tokens[5]);
        }
    }
    return 0;
}