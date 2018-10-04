/*
 * main.cpp
 *
 *  Created on: 25 Sep 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#define MDP_TESTING

#include "../MultibeamDataProcessor/src/Profiler/DepthProfiler.hpp"
#include "../MultibeamDataProcessor/src/Messages/SwitchDataCommand.hpp"
#include "../MultibeamDataProcessor/src/Communication/RequestResponseHandler.hpp"
#include "../MultibeamDataProcessor/src/Messages/SonarReturnDataPacket.hpp"
#include "../MultibeamDataProcessor/src/Messages/SonarReturnData.hpp"
#include "../MultibeamDataProcessor/src/Messages/ProfilePointOutput.hpp"
#include "../MultibeamDataProcessor/src/Controller/DeltaT100/DeltaT100Controller.hpp"
#include "../MultibeamDataProcessor/src/Communication/IOHandler.hpp"
#include "../MultibeamDataProcessor/src/Communication/TCP/TCPConnection.hpp"
#include "../MultibeamDataProcessor/src/FileHandler.hpp"
#include "../MultibeamDataProcessor/src/Filter.hpp"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_LOG_LEVEL all
#define BOOST_TEST_MODULE DeltaT100Communicatie
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <random>

class Test: public Communication::RequestHandler
{
public:
	Test()
	{

	}

	virtual ~Test()
	{

	}

	Communication::SensorMessage handleRequest(uint8_t* data, std::size_t length, std::chrono::milliseconds::rep time = 0)
	{
		std::cout << "We got a request" << std::endl;
		Messages::SensorMessage message(data,length,time);
		Messages::SwitchDataCommand& command = static_cast<Messages::SwitchDataCommand&>(message);
		if(!has_send_request_zero_ && command.getData()[SDC_PACKET_NR_REQEUST_OFFSET] == 0)
		{
			has_send_request_zero_ = true;
			Messages::ProfilePointOutput wrong_message;
			return wrong_message;
		}
		else if(command.getData()[SDC_PACKET_NR_REQEUST_OFFSET] == 0)
		{
			has_resend_ = true;
			Communication::IOHandler::getInstance().stopIOService();
		}
		else
		{
			Communication::IOHandler::getInstance().stopIOService();
		}

		return Messages::SensorMessage(0);
	}

	bool has_send_request_zero_ = false;
	bool has_resend_ = false;

};
BOOST_AUTO_TEST_SUITE( filters )
BOOST_AUTO_TEST_CASE(filter_zero)
{
	Matrix<6,3,float> test_matrix_1;
	test_matrix_1.at(2,0) = 6;
	test_matrix_1.at(3,0) = 5.9;
	test_matrix_1.at(5,0) = 6;
	ZeroFilter::applyFilter(test_matrix_1);
//	std::cout << test_matrix_1 << std::endl;

	Matrix<6,3,float> test_matrix_2;
	test_matrix_2.at(0,0) = 6;
	test_matrix_2.at(1,0) = 6;
	test_matrix_2.at(2,0) = 6;
	test_matrix_2.at(3,0) = 6;
	ZeroFilter::applyFilter(test_matrix_2);
//	std::cout << test_matrix_2 << std::endl;

	Matrix<6,3,float> test_matrix_3;
	test_matrix_3.at(1,0) = 6;
	test_matrix_3.at(4,0) = 5;
	ZeroFilter::applyFilter(test_matrix_3);
//	std::cout << test_matrix_3 << std::endl;

	Matrix<DELTAT100_BEAM_COUNT,3,float> test_matrix_4;

	for (std::size_t i = 0; i < test_matrix_4.getHeight(); ++i) {
		if((std::rand() % 100) < 60 )
		{
			test_matrix_4.at(i,0) = std::sin(i*0.1);
		}
	}
	ZeroFilter::applyFilter(test_matrix_4);
//	std::cout << test_matrix_4 << std::endl;
	BOOST_CHECK_EQUAL((Matrix<6,3,float>() != test_matrix_1), true);
	BOOST_CHECK_EQUAL((Matrix<6,3,float>() != test_matrix_2), true);
	BOOST_CHECK_EQUAL((Matrix<6,3,float>() != test_matrix_3), true);
	BOOST_CHECK_EQUAL((Matrix<DELTAT100_BEAM_COUNT,3,float>() != test_matrix_4), true);
}

BOOST_AUTO_TEST_CASE(filter_peaks)
{
	Matrix<DELTAT100_BEAM_COUNT,3,float> test_matrix;

	for (std::size_t i = 0; i < test_matrix.getHeight(); ++i) {
		test_matrix.at(i,0) = 3;
		if((std::rand() % 100) < 10 )
		{
			test_matrix.at(i,0) = 10;
		}
	}

	PeakFilter::applyFilter(test_matrix);

	for (std::size_t i = 0; i < test_matrix.getHeight(); ++i) {
		BOOST_CHECK_EQUAL((test_matrix.at(i,0) != 10), true);
	}
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(performance_data_write)
{
	FileHandler file_handler("performance_data_write.txt");
	Messages::SonarReturnData data_to_write;
	std::ofstream& stream = file_handler.getOutputStream();
    std::chrono::steady_clock::time_point t1;
    std::chrono::steady_clock::time_point t2;
    t1  = std::chrono::steady_clock::now();
    stream.write((char*)data_to_write.getData(), data_to_write.getDataLength());
    t2  = std::chrono::steady_clock::now();
    std::cout << "Write time(nano seconds): " << std::chrono::duration_cast<std::chrono::nanoseconds>( t2 - t1 ).count() << std::endl;
    BOOST_CHECK_LT(std::chrono::duration_cast<std::chrono::nanoseconds>( t2 - t1 ).count(), 1000000);
}

BOOST_AUTO_TEST_SUITE( accuracy )
BOOST_AUTO_TEST_CASE(profile_point_decode)
{
	//Create a message the sensor can send.
	Messages::ProfilePointOutput ppo_template;

	uint16_t beam_count = 480;
	float start_angle = -60;
	uint16_t actual_start_angle = (start_angle + 180) * 100;
	float angleIncrement = 120.0/beam_count;
	float wave_height = -3.0;

	ppo_template.getData()[PPO_NR_OF_BEAMS_HIGH] = (beam_count >> 8);
	ppo_template.getData()[PPO_NR_OF_BEAMS_LOW] = (beam_count & 0x00FF);

	ppo_template.getData()[PPO_START_ANGLE_HIGH] = (actual_start_angle >> 8);
	ppo_template.getData()[PPO_START_ANGLE_LOW] = (actual_start_angle & 0x00FF);

	ppo_template.getData()[PPO_ANGLE_INCREMENT] = static_cast<uint8_t>(angleIncrement * 100);

	ppo_template.getData()[PPO_RANGE_RESOLUTION_HIGH] = (1 >> 8);
	ppo_template.getData()[PPO_RANGE_RESOLUTION_LOW] = (1 & 0x00FF);

	for (std::size_t i = 0; i < beam_count; ++i) {
		float angle = start_angle - angleIncrement * i;
		float beam_length = wave_height / std::sin(Messages::ProfilePointOutput::toRadians(angle)) * 1000;
//		std::cout << "Set beamlength to: " << beam_length << " for beam angle:" << angle << std::endl;
		uint16_t actual_length = std::abs(beam_length);
		uint8_t byte1 = (actual_length >> 8);
		uint8_t byte2 = (actual_length & 0x00FF);
		ppo_template.getData()[PPO_PROFILE_RANGE_START_HIGH + i * 2] = byte1;
		ppo_template.getData()[PPO_PROFILE_RANGE_START_LOW + i * 2] = byte2;
	}

	//Check if we use the right conversion.
	BOOST_CHECK_EQUAL(ppo_template.getNumberOfBeams(), 480);
	auto result = ppo_template.asMatrix();
	for (std::size_t i = 0; i < beam_count; ++i) {
//		std::cout << result.at(i,0) << "," << result.at(i,1) << "," << result.at(i,2) << std::endl;
//		BOOST_CHECK_CLOSE(abs(result.at(i,1)),wave_height,1);
	}
}

BOOST_AUTO_TEST_CASE(amount_ribbel_check)
{
	try {
		Profiler::DepthProfiler<DELTAT100_BEAM_COUNT,float> depth_profiler(false);
	    Matrix<DELTAT100_BEAM_COUNT, 3, float> test_matrix(5);
	    for (std::size_t i = 0; i < test_matrix.getHeight(); ++i) {
	    	test_matrix.at(i,0) = i;
	    	test_matrix.at(i,1) = 5 + abs(std::sin(i*0.1));
		}

	    float expected_dunes = std::floor((float)DELTAT100_BEAM_COUNT / Messages::ProfilePointOutput::PI / 10);
	    std::cout << "Expected dunes: " << expected_dunes << std::endl;
	    depth_profiler.addRawPoint(test_matrix, Communication::ConnectionInterface::getCurrentTime());
	    BOOST_CHECK_GT((float)depth_profiler.getDepthData().begin()->dunes_.size() / expected_dunes, 0.8);
	    std::cout << "Actual dunes: " << depth_profiler.getDepthData().begin()->dunes_.size() << std::endl;
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}
BOOST_AUTO_TEST_SUITE_END();

BOOST_AUTO_TEST_CASE(acuracy_depth_profiler)
{
	Profiler::DepthProfiler<DELTAT100_BEAM_COUNT,float> depth_profiler(false);
    Matrix<DELTAT100_BEAM_COUNT, 3, float> test_matrix(5);
    Matrix<DELTAT100_BEAM_COUNT,3, float> increase;
    for (std::size_t i = 0; i < test_matrix.getHeight(); ++i) {
    	increase.at(i,0) = 1;
		test_matrix.at(i,0) = i;
		test_matrix.at(i,1) = 5;
		if(i > 240 && i <= 271)
		{
			test_matrix.at(i,1) = 5 + std::sin((i-240)*0.1);
		}
	}
    test_matrix.at(240,1) = 4;
    test_matrix.at(272,1) = 4;

    std::cout << "add first" << std::endl;
    depth_profiler.addRawPoint(test_matrix, Communication::ConnectionInterface::getCurrentTime());
    test_matrix += increase;
    std::cout << "add second" << std::endl;
    depth_profiler.addRawPoint(test_matrix, Communication::ConnectionInterface::getCurrentTime());

    //http://www.wolframalpha.com/widget/widgetPopup.jsp?p=v&id=d56e8a800745244232d295d3eae74aae&title=Area+under+the+Curve+Calculator&theme=green
    //With param sin(x*0.1)+5 van x = 0 to x = 31 equals 174.991
    float error_margin = std::abs(174.991 - depth_profiler.getDepthData().begin()->dunes_.at(0).surface_area_) / 174.991;
    BOOST_CHECK_EQUAL(depth_profiler.getDepthData().begin()->dunes_.size(), (std::size_t)1);
    BOOST_CHECK_LT(error_margin,0.05);
}

//Also verifies the handling of inconsistent x-axis spacing.
BOOST_AUTO_TEST_CASE(performance_depth_profiler)
{
	Profiler::DepthProfiler<DELTAT100_BEAM_COUNT,float> depth_profiler(false);
    std::ifstream matrixInput("C:\\Projecten\\Eclipse-workspace\\MultibeamDataProcessor\\Debug\\output_ssv.txt");
    Matrix<DELTAT100_BEAM_COUNT, 3, float> test(matrixInput, false);
    Matrix<DELTAT100_BEAM_COUNT,3, float> increase;
    for (std::size_t i = 0; i < DELTAT100_BEAM_COUNT; ++i) {
		increase.at(i,0) = 1;
	}
    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    uint64_t total_time = 0;
    std::size_t max_iterations = 100;

    for (std::size_t i = 0; i < max_iterations; ++i) {
        test += increase;
        auto time = Communication::ConnectionInterface::getCurrentTime();
        t1  = std::chrono::high_resolution_clock::now();
        depth_profiler.addRawPoint(test, time);
        t2  = std::chrono::high_resolution_clock::now();
        total_time += std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
	}
    float average = total_time / max_iterations;
    std::cout << "Everage profiler time: " << average <<  std::endl;
    BOOST_CHECK_LT(average, 50000);
}

BOOST_AUTO_TEST_CASE(message_with_unexpected_size)
{
	Test* io_test_class = new Test();
	Communication::TCP::TCPServerClient sensor(Communication::IOHandler::getInstance().getIOService(), "localhost", "103", "101");
	sensor.addRequestHandler(std::unique_ptr<Communication::RequestHandler>(io_test_class));
    Controller::DeltaT100::DeltaT100Controller* controller =
        new Controller::DeltaT100::DeltaT100Controller("localhost", "101", "100");
	Communication::IOHandler::getInstance().startIOService();
	Communication::IOHandler::getInstance().stopIOService();
	BOOST_CHECK_EQUAL(io_test_class->has_resend_,true);
	delete controller;
	delete io_test_class;
}

BOOST_AUTO_TEST_CASE(performance_throughput)
{
	Messages::ProfilePointOutput ppo_template;

	Communication::UDP::UDPServerClient sensor(Communication::IOHandler::getInstance().getIOService(), "localhost", "103", "101");
    Controller::DeltaT100::DeltaT100Controller* controller = new Controller::DeltaT100::DeltaT100Controller("localhost", "101", "100");
	Communication::IOHandler::getInstance().startIOService();

    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
	std::size_t bytes_handled = 0;
    uint64_t total_time = 0;

    for (std::size_t i = 0; i < 10; ++i)
    {
    	t1  = std::chrono::high_resolution_clock::now();
    	Messages::SensorMessage response = controller->handleRequest(ppo_template.getData(), ppo_template.getDataLength(), Communication::ConnectionInterface::getCurrentTime());
        t2  = std::chrono::high_resolution_clock::now();
        total_time += std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        bytes_handled += response.getDataLength();
        bytes_handled += Messages::ProfilePointOutput::command_length_;
	}
	Communication::IOHandler::getInstance().stopIOService();

    float throughput = ((float)bytes_handled) / total_time * std::pow(10,6);
    std::cout << "Throughput: " << throughput << std::endl;
    BOOST_CHECK_GT(throughput, 1000000);
    delete controller;
}


