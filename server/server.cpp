#include "WebsocketServer.h"


#include <iostream>
#include <thread>
#include <asio/io_service.hpp>
#include <unordered_map>
#include <stdint.h>


//The port number the WebSocket server listens on
#define PORT_NUMBER 8080
#define NUMBER_OF_MONTY_INSTANCES 5

#define CLIENT "xJHkjahUAHSKJANBifg"
#define MONTY_STATUS "ASJjndafoIHFSjdkfouH"
#define MONTY_END "SJDSHFnkfsjsnKFKDJkjsifnjJjj"








#define HANDLE_MESSAGE(FROM, CODE)                                                      \
	server.message(                                                                     \
		FROM,                                                                           \
		[&mainEventLoop, &server](ClientConnection conn, const Json::Value& args)       \
		{                                                                               \
			mainEventLoop.post(                                                         \
				[conn, args, &server]()                                                 \
				{                                                                       \
					CODE                                                                \
				}                                                                       \
			);                                                                          \
		}                                                                               \
	);                                                                                  \


#define ON_CONNECT(CODE) \
	server.connect(\
		[&mainEventLoop, &server](ClientConnection conn)\
		{ \
			mainEventLoop.post( \
				[conn, &server]() \
				{ \
					CODE \
				}\
			);\
		}\
	);\


#define ON_DISCONNECT(CODE) \
	server.disconnect( \
		[&mainEventLoop, &server](ClientConnection conn) \
		{ \
			mainEventLoop.post( \
				[conn, &server]() \
				{ \
					CODE \
				} \
			); \
		} \
	); \




#include <chrono>
#include <thread>

using namespace std::this_thread;
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;


int main(int argc, char* argv[])
{
	// create the event loop for the main thread, and the WebSocket server
	asio::io_service mainEventLoop;
	WebsocketServer server;

	ON_CONNECT(
		std::clog << "Connection opened." << std::endl;
		std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;
		
		// send a hello message to the client
		server.sendMessage(conn, "hello", Json::Value());
	)
	
	ON_DISCONNECT(
		std::clog << "Connection closed." << std::endl;
		std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;
	)


	// mock stuff
	HANDLE_MESSAGE("monty",
		Json::Value message1;

		server.sendMessage(conn, "Initializing instance... ", message1);
		sleep_for(1s);

		server.sendMessage(conn, "Preparing materials...", message1);
		sleep_for(5s);

		server.sendMessage(conn, "Generating geometry...", message1);
		sleep_for(1s);

		server.sendMessage(conn, "Putting it all toguether...", message1);
		sleep_for(1s);

		server.sendMessage(conn, "Running simulation, this may take a while depending on your parameters...", message1);
		sleep_for(60s);

		server.sendMessage(conn, "Done.", message1);
		sleep_for(500ms);
	)

	// from client
	HANDLE_MESSAGE(CLIENT,
		bool started = false;

		for (int i; i<NUMBER_OF_MONTY_INSTANCES; ++i){
			/* send /GET request */
			std::clog << "GET /100x" << std::endl;

			if (started){
				/*if available, it has already started: store conn info and return*/
				Json::Value message1;
				server.sendMessage(
					conn, 
					"An instance is now running your simulation.", 
					message1
				);


				return;

			}
		}

		Json::Value message1;
		server.sendMessage(
			conn,
			"All instances are busy.",
			message1
		);
	)

	// from monty container instance
	HANDLE_MESSAGE(MONTY_STATUS, )
	HANDLE_MESSAGE(MONTY_END, )






	
	// Start the networking thread
	std::thread serverThread(
		[&server]() {
			server.run(PORT_NUMBER);
		} // lambda
	); // serverThread


	// Start the event loop for the main thread

	std::cout << "Starting ws... \n"; 
	asio::io_service::work work(mainEventLoop);

	std::cout << "Running ws... \n"; 
	mainEventLoop.run();
	
	std::cout << "ws stopped... \n"; 
	return 0;
}
