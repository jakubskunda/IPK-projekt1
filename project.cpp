// Jakub Skunda xskund02
// VUT FIT 2022
// IPK - projekt1

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include  <bits/stdc++.h>
#include <tuple>
using namespace std;

/**
 * @brief function that returns data necessary for cpu_load
 * 
 * @return std::tuple <int , int, int, int , int, int, int, int > 
 */
std::tuple <int , int, int, int , int, int, int, int > get_data(){
    string name;
    ifstream inFile;
    inFile.open("/proc/stat");
    getline(inFile, name);
    inFile.close();

    string words;
    int data[9];

    stringstream splited(name);
    int nr = 0;

    while (splited >> words){
        if(nr >= 1){
            data[nr-1] = std::stol(words);    
        }        
        nr++;
    }
    
    return make_tuple(data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
}


/**
 * @brief Function that counts actual cpu_load of processor
 * 
 * @param socket 
 */
void cpu_load(int socket){
    
    long int prevuser, prevnice, prevsystem, previdle, previowait, previrq, prevsoftirq, prevsteal;
    tie (prevuser, prevnice, prevsystem, previdle, previowait, previrq, prevsoftirq, prevsteal) = get_data();
    
    usleep(50000);

    long int user, nice, system, idle, iowait, irq, softirq, steal;
    tie (user, nice, system, idle, iowait, irq, softirq, steal) = get_data();
    
    long int PrevIdle = previdle + previowait;
    long int Idle = idle + iowait;

    long int PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal;
    long int NonIdle = user + nice + system + irq + softirq + steal;

    long int PrevTotal = PrevIdle + PrevNonIdle;
    long int Total = Idle + NonIdle;

    long double totald = Total - PrevTotal;
    long double idled = Idle - PrevIdle;

    long double CPU_Percentage = ((totald - idled) / totald)* 100;

    //conversion to char*
    string value = std::to_string(CPU_Percentage)+"%";
    char final_value[value.length() + 1];
    strcpy(final_value, value.c_str());
    char *p = final_value;

    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: ";
    response += std::to_string(strlen(final_value)) + "\r\n\r\n" + final_value;
    write(socket, response.c_str(), response.size());
}   

/**
 * @brief function that find and write info about cpu into socket
 * 
 * @param socket 
 */
void cpu_info(int socket){
    char buffer[200];
    FILE *stream = popen("grep -m 1  'model name' /proc/cpuinfo", "r");
    
    if(stream == NULL){
        return;
    }

    int c;
    int index = 0;
    while((c = fgetc(stream)) != EOF ){
        buffer[index++] = c;
        
    }
    pclose(stream);
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: ";
    response += std::to_string(strlen(buffer)) + "\r\n\r\n" + buffer;
    write(socket, response.c_str(), response.size());
}

/**
 * @brief function that finds and write hostname into socket
 * 
 * @param socket 
 */
void hostname(int socket){
    char hostname[1024];
    gethostname(hostname, 1024);
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: ";
    response += std::to_string(strlen(hostname)) + "\r\n\r\n" + hostname;
    write(socket, response.c_str(), response.size());
}

/**
 * @brief main function
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char const *argv[])
{
    int server_socket, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1000];
    
    // checking args
    if(argc != 2){
        perror("wrong number of arguments");
        exit(EXIT_FAILURE);
    }

    // setting port to argument 1
    std::string port = argv[1];

    // checking if port is number
    for(int i = 0; i < port.length(); i++){
        if(!isdigit(port[i])){
            perror("port should be a number");
            exit(EXIT_FAILURE);
        }
    }

    
    // creating socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
       
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // naviazanie na lokalny port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( std::stoi( port ) );
       

    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_socket, 1) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    char *info;

    // cakanie na data a zapis dat
    while (1){
        if ((new_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        valread = read(new_socket, buffer, 1000);

        if(strncmp(buffer, "GET /load ", 10) == 0){
            cpu_load(new_socket);
        }

        else if(strncmp(buffer, "GET /hostname ", 14) == 0){
            hostname(new_socket);
        }

        else if(strncmp(buffer, "GET /cpu-name ", 14) == 0){
            cpu_info(new_socket);
        }
        else{
            write(new_socket, "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain;\r\n\r\nInternal Server Error", sizeof("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain;\r\n\r\nInternal Server Error"));    
        }
        close(new_socket);
    }
    
    close(server_socket);
    return 0;
}
