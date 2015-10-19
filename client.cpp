
#include <iostream>
#include <vector>
#include <string.h>
#include <unistd.h>  // read関数を使う為

#include <sys/socket.h>  // ソケット通信用 serverとclientの両方で必要
#include <error.h>  // エラー解析用
#include <netinet/in.h>  // sockaddr_in用
#include <arpa/inet.h>  // inet_addr用

// 送信した関数
void Send(std::vector< char >& d, int& our)
{
	// 送信するデータのサイズ
	int ndata = d.size();
	// 送信バッファ
	char sendBuf[ndata];

	// 送信バッファにコピー
	memcpy(sendBuf, &d[0], ndata);

	// 送信
	int mWrite = write(our, sendBuf, sizeof(sendBuf));
	if (mWrite <= 0) perror("send error");
}

int main(int argc, char const* argv[])
{
	// ポート番号
	int port = 9999;
	int our_sock;

	// IPアドレス
	// このテストプログラムではローカルで行っているので以下のアドレス
	// std::string ip = "127.0.0.1";
	std::string ip = "192.168.0.54";

	// AF_INETアドレスファミリーを使用する時は
	// sockaddr_inの構造体を使用する
	struct sockaddr_in client;

	// ソケットの作成
	// server,client共通
	// 今回はTCP/IPで作成するので以下のように引数を与える
	// 引数1:アドレスファミリ AF_INET:IPv4
	// 引数2:ソケットタイプ SOCK_STREAM:TCP
	// 引数3:プロトコル
	our_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (our_sock < 0) {  // 失敗
		std::cout << "socket failed" << std::endl;
	} else
		std::cout << "socket success" << std::endl;

	// クライアント側のポートやアドレス
	client.sin_family = AF_INET;
	client.sin_port = htons(port);

	// ipアドレスの指定
	client.sin_addr.s_addr = inet_addr(ip.c_str());

	// サーバーに接続できたかどうか
	int conServer =
	    connect(our_sock, (struct sockaddr*)&client, sizeof(client));
	if (conServer < 0) perror("connect error");

	std::vector< char > d;
	uint h;

	while (1) {

		std::cout << "input num h = ";
		std::cin >> h;

		d.push_back(h);
		// 接続したサーバーに情報を送信
		Send(d, our_sock);
		d.clear();
		if (h == 0) break;
	}

	std::cout << "client stop" << std::endl;
	close(our_sock);

	return 0;
}
