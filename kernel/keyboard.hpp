/**
 * @file keyboard.hpp
 * 
 * キーボード関係.
 */

#pragma once

#include <deque>
#include "message.hpp"

void InitializeKeyboard(std::deque<Message>& msg_queue);
