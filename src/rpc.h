/*
 * rpc.h
 *
 *  Created on: Oct 1, 2014
 *      Author: francis
 */

#ifndef RPC_H_
#define RPC_H_

enum commands {
	RPC_HOG,
	RPC_SLEEP,
	RPC_PING,
};

struct message {
	int cmd;
	int arg;
	int cnt;
	int ret;
};

#endif /* RPC_H_ */
