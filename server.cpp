#include <Magick++.h>
#include <evhttp.h>

#include <memory>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>

using namespace std;
using namespace Magick;


int main()
{
	if (!event_init())
	{
		cerr << "Failed to init libevent." << endl;
		return -1;
	}

	// Запуск сервера
	const char host[] = "127.0.0.1";
	uint16_t port = 5875;
	unique_ptr<evhttp, decltype(&evhttp_free)> server(evhttp_start(host, port), &evhttp_free);
	
	if (!server)
	{
		cerr << "Failed to init http server." << endl;
		return -1;
	}
	
	void (*onReq)(evhttp_request *req, void *) = [](evhttp_request *req, void *)
	{
		// Записываем входящиее данные
		auto* inBuf = evhttp_request_get_input_buffer(req);
		// Получаем их длинну
		size_t lenOfData = evbuffer_get_length(inBuf);

		//Преобразуем их в char массив
		char* data = static_cast<char*>(malloc(lenOfData + 1));
		evbuffer_copyout(inBuf, data, lenOfData);
		
		// Достаём картинку из данных
		Magick::Blob blob(static_cast<const void *>(data), lenOfData);
		Image image;
		image.read(blob);

		// Отзеркаливаем
		image.flop();    
		// Записываем новый файл
    	image.write("out.jpeg");

		// Готовим данные к отправке
		auto* outBuf = evhttp_request_get_output_buffer(req);
		// Открываем получившуюся картинку
		int fd = open("out.jpeg", O_RDONLY);
		
		if (!outBuf)
		{
			return;
		}
		
		// Получаем длинну
		int len = lseek(fd , 0 , SEEK_END);
		// Добавлеяем в баффер для отправки картинку
		evbuffer_add_file(outBuf, fd, 0, len);
		// Отправляем ответ на запрос
		evhttp_send_reply(req, HTTP_OK, "", outBuf);
	};
	
	evhttp_set_gencb(server.get(), onReq, nullptr);
	
	if (event_dispatch() == -1)
	{
		cerr << "Failed to run message loop." << endl;
		return -1;
	}


  	return 0;
}