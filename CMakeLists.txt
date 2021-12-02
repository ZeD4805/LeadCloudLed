cmake_minimum_required(VERSION 3.17)
project(Lead_Cloud)

set(CMAKE_CXX_STANDARD 14)

add_executable(Lead_Cloud main.cpp utils/DoubleLinkedList.h VisualJobs/DefaultTicketQueue/TicketQueue.cpp VisualJobs/DefaultTicketQueue/TicketQueue.h VisualJobs/DefaultTicketQueue/Ticket.h VisualJobs/DefaultTicketQueue/TicketQueueConfig.h VisualJobs/VisualJobConfig.h VisualJobs/VisualJobConfig.cpp LedDriver/LedDriver.cpp LedDriver/LedDriver.h)