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

#ifndef SIO_CLIENT_HANDLER_HPP
#define SIO_CLIENT_HANDLER_HPP

// com.zaphoyd.websocketpp.chat protocol
// 
// client messages:
// alias [UTF8 text, 16 characters max]
// msg [UTF8 text]
// 
// server messages:

#include <boost/shared_ptr.hpp>

//#include "roles/client.hpp"
#include "roles/sioclient.hpp"
#include "websocketpp.hpp"
#include <include/json/json.h>

#include <map>
#include <string>
#include <queue>

using websocketpp::sioclient;

namespace websocketpp{

class sio_client_handler : public sioclient::handler {
public:
	
    sio_client_handler() {
		
	}
    virtual ~sio_client_handler() {}

	void on_connect(connection_ptr con) {

		con->alog().at(log::alevel::DEVEL)<<"Connection on_connect"<<  log::endl;
	}

    void on_fail(connection_ptr con) {

		con->alog().at(log::alevel::DEVEL)<<"Connection Connection connected "<<  log::endl;
	}

	void on_open(connection_ptr con) {
		m_con = con;

		con->alog().at(log::alevel::DEVEL)<<"Successfully connected sid "<<  con->m_endpoint.m_strSid <<log::endl;
	}

	void on_close(connection_ptr con) {
		m_con = connection_ptr();
		con->stop_heartbeat();

		con->alog().at(log::alevel::DEVEL)<<"client was disconnected sid "<<  con->m_endpoint.m_strSid <<log::endl;

	}

    
   
    
    // CLIENT API
	void send(const std::string &msg){
		if (!m_con) {
			throw exception("not connnect");
		}
		m_con->alog().at(log::alevel::DEVEL)<<"send: "<<  msg <<log::endl;
		m_con->send(msg);
	};
	void send(Json::Value &v){
		Json::FastWriter w;
		send("4:::" +w.write(v));
	};

	//发送一条消息
	void emit(const std::string &eventName,const std::string &data){
		send("5:::{\"name\":\""  + eventName + "\",\"args\":"+data +"}");
	};

	void emit(const std::string &eventName,Json::Value  &data){
		Json::Value v;
		Json::FastWriter w;
		v["name"] = eventName;
		v["args"] = data;
		send("5:::" + w.write(v));
	};
	void on_event(const std::string &eventName,Json::Value data,message_ptr msg){
			m_con->alog().at(log::alevel::DEVEL)<<"on_event: "<<  eventName <<" "<<data.toStyledString() <<log::endl;
	};
	void on_event(const std::string &eventName,message_ptr msg){

	};

	void on_strmessage(const std::string &strMessage,message_ptr msg){
		m_con->alog().at(log::alevel::DEVEL)<<"on_strmessage: "<<  strMessage <<log::endl;
	};
	void on_jsonmessage(const std::string &jsonMessage,message_ptr msg){
		m_con->alog().at(log::alevel::DEVEL)<<"on_jsonmessage: "<<  jsonMessage <<log::endl;
	}
	



void close() {
    if (!m_con) {
        std::cerr << "Error: no connected session" << std::endl;
        return;
    }
    m_con->close(websocketpp::close::status::GOING_AWAY,"");
}


	

private:
    void decode_server_msg(const std::string &msg);
	// got a new message from server
	void on_message(connection_ptr con, message_ptr msg){
		//https://github.com/LearnBoost/socket.io-spec#Encoding
		/*	0		Disconnect
		1::	Connect
		2::	Heartbeat
		3:: Message
		4:: Json Message
		5:: Event
		6	Ack
		7	Error
		8	noop
		*/
		std::string strm = msg->get_payload();
		char f = strm[0];
		std::string::size_type p = 1;
		for(int i = 0 ; i < 2; i++){
			p = strm.find_first_of(':',p+1);
		}
		std::string data = strm.substr(p+1);
		con->alog().at(log::alevel::DEVEL)<<"revc: "<<  strm <<log::endl;
		con->alog().at(log::alevel::DEVEL)<<"type: "<< f <<" data: "<<  data <<log::endl;
		Json::Reader r;
		Json::Value v;
		switch(f){
		case '0':
			close();
			break;
		case '1':
			on_connect(con);
			break;
		case '2':
			con->send_heartbeat();
			break;
		case '3':
			on_strmessage(data,msg);
			break;
		case '4':
			on_jsonmessage(data,msg);
			/*
			var fe:FlashSocketEvent = new FlashSocketEvent(FlashSocketEvent.MSG_JSON);
			fe.data = JSON.decode(dm.msg);
			dispatchEvent(fe);
			*/
			break;
		case '5':

			if(r.parse(data,v)){
				on_event(v["name"].asString(),v["args"],msg);
			}else{
				con->alog().at(log::alevel::DEVEL)<<"parse json failed error  "<<  data <<log::endl;
			}
			/*
			var m:Object = JSON.decode(dm.msg);
			var e:FlashSocketEvent = new FlashSocketEvent(FlashSocketEvent.EVENT);
			e.data = m;
			dispatchEvent(e);
			*/
			break;
		case '7':
			//on_fail();	
			/*
			var m:Object = JSON.decode(dm.msg);
			var e:FlashSocketEvent = new FlashSocketEvent(FlashSocketEvent.CONNECT_ERROR);
			e.data = dm.msg;
			dispatchEvent(e);
			*/

			break;
		default:
			break;
		}
	}

    
    // list of other chat participants
    std::set<std::string> m_participants;
    std::queue<std::string> m_msg_queue;
    connection_ptr m_con;
};

typedef boost::shared_ptr<sio_client_handler> sio_client_handler_ptr;

}
#endif // sio_client_handler_HPP
