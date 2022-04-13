#include "WebsocketServer.h"

#include <iostream>
#include <thread>
#include <asio/io_service.hpp>
#include <unordered_map>
#include <stdint.h>

//The port number the WebSocket server listens on
#define PORT_NUMBER 8080

#include <chrono>
#include <thread>

using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;



class Monty{
	ClientConnection conn;

	bool has_been_used;

	public:
		Monty(){}
		Monty(ClientConnection conn){
			this->conn = conn;
			this->has_been_used = false;
		}


		void process(WebsocketServer &server){


			Json::Value message1;

			server.sendMessage(this->conn, "Initializing instance... ", message1);
    		sleep_for(1s);


			server.sendMessage(this->conn, "Preparing materials...", message1);
    		sleep_for(5s);



			server.sendMessage(this->conn, "Generating geometry...", message1);
    		sleep_for(1s);


			server.sendMessage(this->conn, "Putting it all toguether...", message1);
    		sleep_for(1s);


			server.sendMessage(this->conn, "Running simulation, this may take a while depending on your parameters...", message1);
			sleep_for(60s);


			server.sendMessage(this->conn, "Done.", message1);
    		sleep_for(500ms);

			this->has_been_used = true;

		} /*std::string getMessage()*/
	/*public*/
};


int main(int argc, char* argv[])
{
	//Create the event loop for the main thread, and the WebSocket server
	asio::io_service mainEventLoop;
	WebsocketServer server;
	
	//Register our network callbacks, ensuring the logic is run on the main thread's event loop
	server.connect(
		[&mainEventLoop, &server](ClientConnection conn)
		{
			mainEventLoop.post(
				[conn, &server]()
				{
					std::clog << "Connection opened." << std::endl;
					std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;
					
					//Send a hello message to the client
					server.sendMessage(conn, "hello", Json::Value());
				}
			);
		}
	);


	server.disconnect(
		[&mainEventLoop, &server](ClientConnection conn)
		{
			mainEventLoop.post(
				[conn, &server]()
				{
					std::clog << "Connection closed." << std::endl;
					std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;
				}
			);
		}
	);

	server.message(
		"message", 
		[&mainEventLoop, &server](ClientConnection conn, const Json::Value& args)
		{
			mainEventLoop.post(
				[conn, args, &server]()
				{
					std::clog << "message handler on the main thread" << std::endl;
					std::clog << "Message payload:" << std::endl;

					for (auto key : args.getMemberNames()) {
						std::clog << "\t" 
								  << key 
								  << ": " 
								  << args[key].asString() 
								  << std::endl;
					}
					
					//Echo the message pack to the client
					server.sendMessage(conn, "message", args);
				}
			);
		}
	);


	ClientConnection conn;
	Monty monty_instance;





	std::unordered_map<intptr_t, Monty> monty_instances;
	ClientConnection _conn;

	monty_instances
		[(intptr_t) (&_conn)]
				= Monty();


	server.message(
		"monty", 
		[&mainEventLoop, &server, &monty_instances](ClientConnection conn, const Json::Value& args)
		{
			mainEventLoop.post(
				[conn, args, &server, &monty_instances]()
				{
					std::clog << "Handling Monty message..." << std::endl;
					std::clog << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << &conn << std::endl;

					if ( monty_instances.count((intptr_t) (&conn)) == 0 ){
						std::clog << ">>>  Creating new instance..." << std::endl;
						monty_instances[(intptr_t) (&conn)] = Monty(conn);
					}


					Monty *monty = &monty_instances[(intptr_t) (&conn)];

					monty->process(server);




				}
			);
		}
	);
	
	//Start the networking thread
	std::thread serverThread(
		[&server]() {
			server.run(PORT_NUMBER);
		}
	);
	
	//Start a keyboard input thread that reads from stdin
	std::thread inputThread(
		[&server, &mainEventLoop]()
		{
			string input;
			while (1)
			{
				//Read user input from stdin
				std::getline(std::cin, input);
				
				//Broadcast the input to all connected clients (is sent on the network thread)
				Json::Value payload;
				payload["input"] = input;
				server.broadcastMessage("userInput", payload);
				
				//Debug output on the main thread
				mainEventLoop.post(
					[]() {
						std::clog << "User input debug output on the main thread" << std::endl;
					}
				);
			}
		}
	);
	
	//Start the event loop for the main thread

	std::cout << "Starting server... \n"; 
	asio::io_service::work work(mainEventLoop);

	std::cout << "Running server... \n"; 
	mainEventLoop.run();
	
	std::cout << "Server stopped... \n"; 
	return 0;
}
