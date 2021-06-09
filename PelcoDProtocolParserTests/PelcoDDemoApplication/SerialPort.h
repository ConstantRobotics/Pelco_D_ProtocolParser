#pragma once
/**
 * \file Header file
 */
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#if defined(linux) || defined(__linux) || defined(__linux__)
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/file.h>
#include <stdint.h>
#include <mutex>
#else
#include <windows.h>
#endif

namespace clib
{

	/**
	 * \brief Serial port class.
	 */
	class SerialPort {
	public:

		/**
		 * @brief Class constructor.
		 */
		SerialPort();

		/**
		 * @brief Class destructor.
		 */
		~SerialPort();

		/**
		 * @brief Method to open serial port.
		 * @param comport_file Serial port name string. Format depends from OS.
		 * @param baudrate Boudrate.
		 * @param timeout Wait data timeout.
		 * @param mode Mode. Always 3 simbols: 1 - Number of bits (8, 7, 6, 5), 2 - parity (N, E, O), 3 - number of stop bits (1 or 2). Example: "8N1".
		 * @return TRUE in case success or FALSE in case any errors.
		 */
		bool Open(
			const char *comport_file,
			unsigned int baudrate,
			unsigned int timeout = 100,
			const char *mode = "8N1");

		/**
		 * @brief Method to read data from serial port.
		 * @param buf pointer to data buffer to copy.
		 * @param size size of data buffer.
		 * @return Number of readed bytes or returns -1.
		 */
		int ReadData(uint8_t *buf, uint32_t size);

		/**
		 * @brief Method to send data.
		 * @param buf pointer to data to send.
		 * @param size size of data to send.
		 * @return Number of bytes sended or return -1 in case any errors.
		 */
		int SendData(uint8_t *buf, uint32_t size);

		/**
		 * \brief Method to close serial port.
		 */
		void Close();
		/**
		 * \brief Method to check if serial port open.
		 */
		bool isOpen();

	private:
		
		///< Port initialization flag.
		bool initFlag;
		///< Port handle.
#if defined(linux) || defined(__linux) || defined(__linux__)|| defined(__FreeBSD__)
		int Cport;
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
		HANDLE Cport;
#endif

	};

}