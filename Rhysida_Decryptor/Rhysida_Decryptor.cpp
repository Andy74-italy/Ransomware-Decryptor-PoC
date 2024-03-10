// Rhysida_Decryptor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <fstream>
#include <tomcrypt.h>

unsigned char* readDimFromFile(std::istream& fileInput, int offset) {
    fileInput.seekg(-4 - offset, std::ios::end);
    unsigned char buffer[4];
    fileInput.read(reinterpret_cast<char*>(buffer), 4);
    return buffer;
}

unsigned char* readFile(std::istream& fileInput, int dimension, int offset) {
    fileInput.seekg(0 - dimension - offset, std::ios::end);
    unsigned char* buffer = (unsigned char*)malloc(sizeof(unsigned char) * dimension);
    fileInput.read(reinterpret_cast<char*>(buffer), dimension);
    return buffer;
}

void processFileEnc(char* file_name)
{
    /**********************************************************************/
    /* #pragma region preparing files and initializing the randomizer     */
    std::ifstream fileInput(file_name, std::ios::binary);
    if (!fileInput.is_open()) {
        std::cerr << "Cannot open file: " << file_name << std::endl;
        return;
    }
    int filenameoriginallenght = strlen(file_name) - strlen(".rhysida");
    char* originalFileName = (char*)malloc(sizeof(char) * filenameoriginallenght);
    std::copy(file_name, file_name + filenameoriginallenght, originalFileName);
    originalFileName[filenameoriginallenght] = '\0';
    std::ofstream fileOutput(originalFileName, std::ios::binary);
    if (!fileOutput.is_open()) {
        std::cerr << "Cannot create file: " << originalFileName << std::endl;
        return;
    }
    struct stat fileInfo;
    if (stat(file_name, &fileInfo) != 0) {
        std::cerr << "Cannot read file info." << std::endl;
        return;
    }
    /* #pragma endregion preparing files                                  */
    /**********************************************************************/

    /**********************************************************************/
    /* #pragma region Calculate extra data dimension                      */
    int offset = 0;
    unsigned char* bufferRead = readDimFromFile(fileInput, offset);
    int curType_n = 0;
    std::memcpy(&curType_n, bufferRead, sizeof(curType_n));
    offset += 4;

    bufferRead = readDimFromFile(fileInput, offset);
    int numBytesToRead = 0;
    std::memcpy(&numBytesToRead, bufferRead, sizeof(numBytesToRead));
    offset += 4;
    unsigned char* firstBufferRead = readFile(fileInput, numBytesToRead, offset);
    offset += numBytesToRead;

    bufferRead = readDimFromFile(fileInput, offset);
    numBytesToRead = 0;
    std::memcpy(&numBytesToRead, bufferRead, sizeof(numBytesToRead));
    offset += 4;
    unsigned char* secondBufferRead = readFile(fileInput, numBytesToRead, offset);
    offset += numBytesToRead;

    offset = static_cast<long long>(static_cast<std::streampos>(fileInfo.st_size)) - offset;
    /* #pragma endregion Calculate extra data dimension                   */
    /**********************************************************************/

    fileInput.seekg(0, std::ios::beg);
    unsigned char* bufferIn = (unsigned char*)malloc(sizeof(unsigned char) * offset);
    fileInput.read(reinterpret_cast<char*>(bufferIn), offset);
    unsigned char* bufferOut = (unsigned char*)malloc(sizeof(unsigned char) * offset);

    for (int i = 0; i < 120; i++)
    {
        int rand_count = 0;
        std::cout << i << ":";

        /**********************************************************************/
        /* #pragma region initialize the decription engine                    */
        std::srand(static_cast<unsigned int>(fileInfo.st_mtime - i));

        time_t tempo = fileInfo.st_mtime - i;
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%c", std::localtime(&tempo));
        std::cout << buffer << " - ";
        time_t test = static_cast<unsigned>(std::time(nullptr));

        for (int j = 0; j < 3; j++)
        {
            int result;
            int* in_RDX = new int;
            unsigned char prng_entr[0x28];
            prng_state* in_RCX = new prng_state();
            int iVar1 = register_prng(&chacha20_prng_desc);
            *in_RDX = iVar1;
            if ((result = chacha20_prng_start(in_RCX)) != CRYPT_OK)
            {
                std::cerr << "ERROR starting prng: " << error_to_string(result) << std::endl;
                return;
            }
            if ((result = chacha20_prng_ready(in_RCX)) != CRYPT_OK)
            {
                std::cerr << "ERROR checking readyness of prng: " << error_to_string(result) << std::endl;
                return;
            }
            for (int k = 0; k < 0x28; k = k + 1) {
                prng_entr[k] = ((char)k + (char)*in_RDX + '\x01') * (char)(rand() % 0x100);
                rand_count++;
            }
            if ((result = chacha20_prng_add_entropy(prng_entr, 0x28, in_RCX)) != CRYPT_OK)
            {
                std::cerr << "ERROR adding entropy prng: " << error_to_string(result) << std::endl;
                return;
            }
            unsigned char* _Memory = (unsigned char*)malloc(rand() % 0x100 + 1);
            rand_count++;
            chacha20_prng_read(_Memory, 8, in_RCX);
            /* #pragma endregion initialize the decription engine                 */
            /**********************************************************************/

            /**********************************************************************/
            /* #pragma region try to decrypt message                              */
            if ((result = register_all_ciphers()) != CRYPT_OK)
            {
                std::cerr << "ERROR registering all ciphers: " << error_to_string(result) << std::endl;
                return;
            }
            if ((result = register_all_hashes()) != CRYPT_OK)
            {
                std::cerr << "ERROR registering all hashes: " << error_to_string(result) << std::endl;
                return;
            }
            int CIPHER = find_cipher("aes");
            unsigned char* cipher_IV = (unsigned char*)malloc(sizeof(unsigned char) * 0x20);
            unsigned char* local_e0 = (unsigned char*)malloc(sizeof(unsigned char) * 0x10);
            chacha20_prng_read(cipher_IV, 0x20, in_RCX);
            chacha20_prng_read(local_e0, 0x10, in_RCX);
            symmetric_CTR cipher_IV_out_length;
            if ((result = ctr_start(CIPHER, local_e0, cipher_IV, 0x20, 0xe, 0, &cipher_IV_out_length)) != CRYPT_OK)
            {
                std::cerr << "ERROR starting the cipher: " << error_to_string(result) << std::endl;
                return;
            }
            if ((result = ctr_setiv(local_e0, 0x10, &cipher_IV_out_length)) != CRYPT_OK)
            {
                std::cerr << "ERROR setting the IV: " << error_to_string(result) << std::endl;
                return;
            }
            if ((result = ctr_decrypt(bufferIn, bufferOut, offset, &cipher_IV_out_length)) != CRYPT_OK)
            {
                std::cerr << "ERROR decrypting the file content: " << error_to_string(result) << std::endl;
                return;
            }

            if (std::strstr(reinterpret_cast<const char*>(bufferOut), "Alighieri") != nullptr) {
                std::cout << "Data decrypted: i=" << i << ", j=" << j << std::endl;
                // std::cout << std::endl << std::endl << bufferOut << std::endl;
                fileOutput << bufferOut << std::endl;
                fileInput.close();
                fileOutput.close();
                exit(0);
            }
            std::cout << " try rand #" << j << " - ";
            /* #pragma endregion try to decrypt message                           */
            /**********************************************************************/
            zeromem(in_RDX, sizeof(in_RDX));
            zeromem(in_RCX, sizeof(in_RCX));
            zeromem(_Memory, sizeof(_Memory));
            zeromem(cipher_IV, sizeof(cipher_IV));
            zeromem(local_e0, sizeof(local_e0));
        }
        std::cout << " ..... number of time rand function called = " << rand_count << std::endl;
    }
    fileInput.close();
    fileOutput.close();
}

int main()
{
    // const char* constfilename = "C:\\temp\\wikipedia-divinacommedia.txt.rhysida";
    const char* constfilename = "C:\\temp\\divina_commedia.txt.rhysida";
    processFileEnc((char*)constfilename);
}