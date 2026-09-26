/*
File: ipv4.cpp
Description: Extracts and validates an IPv4 address with an optional port
             from a line of text.
Author: Nick Heyer
Created: 9/26/26
*/

#include <iostream>
#include <string>
#include <cctype>

using namespace std;


// checks if a character could be part of an IPv4 token
bool isTokenChar(char c)
{
    return isdigit(c) || c == '.' || c == ':';
}


// validates one complete candidate token
bool validateToken(const string& token,
                   unsigned long& address,
                   int& port)
{
    int octets[4];
    int pos = 0;
    int length = token.length();

    // read the four octets
    for (int part = 0; part < 4; part++)
    {
        if (pos >= length || !isdigit(token[pos]))
            return false;

        int start = pos;
        int value = 0;
        int digits = 0;

        // manually convert digits into a number
        while (pos < length &&
               isdigit(token[pos]) &&
               digits < 3)
        {
            value = value * 10 + (token[pos] - '0');
            pos++;
            digits++;
        }

        // more than 3 digits in an octet
        if (pos < length && isdigit(token[pos]))
            return false;


        // AI error was digits > 2 when should be 1 
        // Checks for leading zeros in an octet, if there are more than 1 digit
        // and it starts with a 0 the octet is invalid
        if (digits > 1 && token[start] == '0')
        return false;

        // AI error was 256 and should be 255 
        // IPv4 octets can only have values from 0 to 255 anything greater than 255 is invalid
        if (value > 255)
            return false;


        octets[part] = value;

        // first three octets must be followed by periods
        if (part < 3)
        {
            if (pos >= length || token[pos] != '.')
                return false;

            pos++;
        }
    }


    // no port was included
    if (pos == length)
    {
        port = -1;
    }

    // optional port
    else
    {
        if (token[pos] != ':')
            return false;

        pos++;

        if (pos >= length || !isdigit(token[pos]))
            return false;

        int portStart = pos;
        int portValue = 0;
        int digits = 0;

        while (pos < length &&
               isdigit(token[pos]) &&
               digits < 5)
        {
            portValue = portValue * 10 + (token[pos] - '0');
            pos++;
            digits++;
        }

        // port has more than 5 digits
        if (pos < length && isdigit(token[pos]))
            return false;

        // port cannot contain a leading zero
        if (digits > 1 && token[portStart] == '0')
            return false;

        // AI error was 65536 when it should be 65535
        // port numbers are only from 0 to 65535 and anything greater is invalid
        if (portValue > 65535)
            return false;


        // anything remaining means the whole token is invalid
        if (pos != length)
            return false;

        port = portValue;
    }


    // combine the four octets into one 32-bit value
    address =
        ((unsigned long)octets[0] << 24) |
        ((unsigned long)octets[1] << 16) |
        ((unsigned long)octets[2] << 8) |
        (unsigned long)octets[3];

    return true;
}


// searches a complete line for a valid IPv4 address
bool extractIPv4(const string& str,
                 unsigned long& outAddress,
                 int& outPort)
{
    outAddress = 0;
    outPort = -1;

    int i = 0;

    while (i < (int)str.length())
    {
        // skip garbage characters
        if (!isTokenChar(str[i]))
        {
            i++;
            continue;
        }

        int start = i;

        // get the entire candidate token
        while (i < (int)str.length() &&
               isTokenChar(str[i]))
        {
            i++;
        }

        string token = str.substr(start, i - start);

        unsigned long address;
        int port;

        if (validateToken(token, address, port))
        {
            outAddress = address;
            outPort = port;
            return true;
        }
    }

    return false;
}


int main()
{
    string input;

    while (true)
    {
        cout << "Enter text: ";
        getline(cin, input);

        if (input == "END")
        {
            cout << "Program terminated." << endl;
            break;
        }

        unsigned long address;
        int port;

        if (extractIPv4(input, address, port))
        {
            unsigned long a = (address >> 24) & 255;
            unsigned long b = (address >> 16) & 255;
            unsigned long c = (address >> 8) & 255;
            unsigned long d = address & 255;

            cout << "Extracted IPv4 address: "
                 << a << "."
                 << b << "."
                 << c << "."
                 << d
                 << " (decimal value: "
                 << address
                 << ", port: ";

            if (port == -1)
                cout << "none";
            else
                cout << port;

            cout << ")" << endl;
        }
        else
        {
            cout << "No valid IPv4 address found." << endl;
        }
    }

    return 0;
}