#pragma once
// FUNCTIONS TO SEND STATE DATA
void send_stop() { quick_transmit(&bt_i, 's'); }

void send_go() { quick_transmit(&bt_i, 'g'); }

void send_east() { quick_transmit(&bt_i, 'e'); }

void send_west() { quick_transmit(&bt_i, 'w'); }

void send_emergency() { quick_transmit(&bt_i, 'm'); }

void send_not_emergency() { quick_transmit(&bt_i, 'n'); }

void send_doors_open() { quick_transmit(&bt_i, 'o'); }

void send_doors_closed() { quick_transmit(&bt_i, 'c'); }