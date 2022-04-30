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

using namespace std::this_thread;
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;


int main(int argc, char* argv[])
{
	// create the event loop for the main thread, and the WebSocket server
	asio::io_service mainEventLoop;
	WebsocketServer server;
	
	// register our network callbacks, ensuring the logic is run on the main thread's event loop
	server.connect(
		[&mainEventLoop, &server](ClientConnection conn)
		{
			mainEventLoop.post(
				[conn, &server]()
				{
					std::clog << "Connection opened." << std::endl;
					std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;
					
					// send a hello message to the client
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
		"monty", 
		[&mainEventLoop, &server](ClientConnection conn, const Json::Value& args)
		{
			mainEventLoop.post(
				[conn, args, &server]()
				{
					std::thread worker(
						[conn, &server](){

							/* MOCK STUFF */
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

							/////////////////////////////////////////////////////////
							/*THE PLAN*/
							
							
							/*CREATE A FOLDE UNIQUE TO THIS THREAD*/
    						// std::string uuid = uuids::to_string (uuids::uuid_system_generator{}());
							// std::string mkdir_command = std::format("mkdir {}", uuid);

							// system(mkdir_command);


							/*START DOCKER CONTAINER THAT CAN ACCESS THAT FOLDER*/
							// system(rundocker_command)


							/* CHECK FOLDER FOR RESULTS */
							// while (1) check_results

							/* send them over the ws connection */

						}
					);

					worker.detach();

				} // lambda
			); // mainEventLoop.post
		} // lambda
	); // server.message
	
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
