/*
* Copyright (c) 2011, Peter Thorson. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the WebSocket++ Project nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
* ARE DISCLAIMED. IN NO EVENT SHALL PETER THORSON BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
*/

#include "sio_client_handler.hpp"


#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <string>

using boost::asio::ip::tcp;
using websocketpp::sioclient;
using namespace websocketpp;


using namespace std;

class notsupport_exception : public std::exception {
public: 
	notsupport_exception(const std::string& msg) : m_msg(msg) {}
	~notsupport_exception() throw() {}

	virtual const char* what() const throw() {
		return m_msg.c_str();
	}
private:
	std::string m_msg;
};
//cid=482278007_3111144523&name=test110
int handshake(const string &host,const string &str_port,const string &res ){
	
	boost::asio::io_service io_service;

	// Get a list of endpoints corresponding to the server name.
	tcp::resolver resolver(io_service);
	tcp::resolver::query query(host, str_port);
	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

	// Try each endpoint until we successfully establish a connection.
	tcp::socket socket(io_service);
	boost::asio::connect(socket, endpoint_iterator);

	// Form the request. We specify the "Connection: close" header so that the
	// server will close the socket after transmitting the response. This will
	// allow us to treat all data up until the EOF as the content.
	boost::asio::streambuf request;
	std::ostream request_stream(&request);
	request_stream << "POST " << res  << " HTTP/1.0\r\n";
	request_stream << "Host: " << host << "\r\n";
	request_stream << "Accept: */*\r\n";
	request_stream << "Connection: close\r\n\r\n";

	// Send the request.
	boost::asio::write(socket, request);

	// Read the response status line. The response streambuf will automatically
	// grow to accommodate the entire line. The growth may be limited by passing
	// a maximum size to the streambuf constructor.
	boost::asio::streambuf response;
	boost::asio::read_until(socket, response, "\r\n");

	// Check that response is OK.
	std::istream response_stream(&response);
	std::string http_version;
	response_stream >> http_version;
	unsigned int status_code;
	response_stream >> status_code;
	std::string status_message;
	std::getline(response_stream, status_message);
	if (!response_stream || http_version.substr(0, 5) != "HTTP/")
	{
		std::cout << "Invalid response\n";
		return 1;
	}
	if (status_code != 200)
	{
		std::cout << "Response returned with status code " << status_code << "\n";
		return 1;
	}

	// Read the response headers, which are terminated by a blank line.
	boost::asio::read_until(socket, response, "\r\n\r\n");
	

	//*
	std::string header;
	while (std::getline(response_stream, header) && header.substr(0,1) != "\r"){
		//std::cout << header << "\n";
	}
	//std::cout << "\n";
    //*/
	// Write whatever content we already have to output.
	//if (response.size() > 0)
	//	std::cout << &response;
	std::ostringstream osu;
	osu<<"ws://"<<host<<":"<< str_port << "/socket.io/1/flashsocket/";
	char cbuf[1024];
	memset(cbuf,0,sizeof(cbuf));
	response_stream.getline(cbuf,sizeof(cbuf),':');
	std::cout <<"sid: "<<cbuf <<endl;
	osu << cbuf;

	int nHeatbeat;
	response_stream >> nHeatbeat;
	cout <<"nHeatbeat: "<<nHeatbeat <<endl;

	
	std::getline(response_stream, header);
	if(header.find("websocket") == std::string::npos){
		throw notsupport_exception(header);
	}

	

	

	// Read until EOF, writing data to output as we go.
	boost::system::error_code error;
	while (boost::asio::read(socket, response,
		boost::asio::transfer_at_least(1), error))
		std::cout << &response<<endl;
	if (error != boost::asio::error::eof)
		throw boost::system::system_error(error);
}

void print(const boost::system::error_code& /*e*/,
		   boost::asio::deadline_timer* t, int* count)
{
	if (*count < 10)
	{
		std::cout << *count << "\n";
		++(*count);
		 t->expires_at(t->expires_at() + boost::posix_time::seconds(1));
		 t->async_wait(boost::bind(print,boost::asio::placeholders::error, t, count));
	}

}

int main(int argc, char* argv[]) {
	std::string uri;

	if (argc != 2) {
		std::cout << "Usage: `sio_client ws_uri`" << std::endl;
	} else {
		uri = argv[1];
	}


	try {
		/*
		std::cout<<"url: "<<uri<<endl;
		websocketpp::uri u(uri);
		//websocketpp::uri u("http://192.168.1.50:8880/socket.io/1/?t=1332823965725");
		std::cout<<"host: "<<u.get_host()<<"\nport: "<<u.get_port()<<"\npath: "<<u.get_resource()<<endl;

		//return 0;
		handshake(u.get_host(),u.get_port_str(),u.get_resource());
		return 0;
		*/
		sio_client_handler_ptr handler(new sio_client_handler());
		sioclient::connection_ptr con;
		sioclient endpoint(handler);
	

		
		endpoint.alog().set_level(websocketpp::log::alevel::ALL);
		endpoint.elog().set_level(websocketpp::log::elevel::ALL);

		/*
		endpoint.elog().set_level(websocketpp::log::elevel::RERROR);
		endpoint.elog().set_level(websocketpp::log::elevel::FATAL);
		*/

		con = endpoint.get_connection(uri);
		
		

		con->add_request_header("User Agent","WebSocket++/0.2.0 WebSocket++Chat/0.2.0");
		con->add_subprotocol("com.zaphoyd.websocketpp.chat");

		con->set_origin("http://zaphoyd.com");


		endpoint.connect(con);
		Json::Value v;
		v["int"] = 1;
		v["string"] = "string";
		std::cout<<v.toStyledString()<<std::endl;

		
		

		boost::thread th(boost::bind(&sioclient::run, &endpoint, false));

		char line[512];
		while (std::cin.getline(line, 512)) {
			std::string arstr = "{\"t\":1,\"c\":\"";
			arstr= arstr+line+"\"}";
			if(line == "/close"){
				handler->close();
			}
			handler->emit("message",arstr);
		}

		th.join();
	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	return 0;
}
