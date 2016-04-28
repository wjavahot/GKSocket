# GKSocket
长链接客户端
报文结构
因为我们推送的消息包含很多重信息，每条报文消息用bulk string或者integer都不能描述清楚，所以规定每条报文都必须是Array的。
每条报文包含的信息次序是固定的，而不同指令的消息结构是不一样的。
所有消息的结构如下表所示(不同消息字段会有所删减)：
顺序	字段	类型	描述
0	CMD	integer	消息指令
1	LENGTH	integer	内容长度
2	TOKEN&SECRET	string	认证Token与Secret的复合字符串
3	SESSION_ID	string	认证Session
4	HEARTBEAT	integer	长连接心跳周期（单位：秒）
5	VERSION	string	协议版本号
6	NET_TYPE	integer	网络类型
7	CHECKSUM	string	消息内容校检，一般使用MD5
8	SEQ	integer	消息序列
9	TOTAL	integer	总共包数
10	CONTENT	string	消息内容，通常格式为JSON
报文命令
每条消息都必须制定特定的指令标识，放在每条消息的第一个序号位，消息指令集如下表所示：
指令	指令代码	描述
SUB	0x01	注册，客户端必须发起订阅才可以接收推送
BS	0x02	心跳
RE_SUB	0xFE	注册，回复信息
RE_BS	0xFD	心跳，回复信息
注册指令集(SUB与RE_SUB)
订阅指令消息结构如下：
顺序	字段
0	CMD
1	LENGTH
2	TOKEN&SECRET
3	HEARTBEAT
4	VERSION
5	NET_TYPE
6	CONTENT
其中，订阅的CONTENT为空字符串。
订阅回复指令消息结构如下：
顺序	字段
0	CMD
1	LENGTH
2	SESSION_ID
3	HEARTBEAT
4	VERSION
5	CONTENT
其中，订阅成功的话，会返回分配的SESSION_ID，不成功的话SESSION_ID为空，而CONTENT订阅成功的时候为空字符串，不成功的时候返回错误信息。
心跳指令集(BS和RE_BS)
心跳与心跳回复比较简单，CONTENT都为空，实际两个都是空信息指令。
心跳消息结构如下：
顺序	字段
0	CMD
1	LENGTH
2	TOKEN&SECRET
3	SESSION_ID
4	HEARTBEAT
5	VERSION
6	NET_TYPE
7	CONTENT
心跳回复消息结构如下：
顺序	字段
0	CMD
1	LENGTH
2	SESSION_ID
3	HEARTBEAT
4	VERSION
5	CONTENT
网络类型(NET TYPE)
消息中的网络类型是指手机终端的网络环境，比如4G、WIFI等等：
网络类型	指令代码
2G	0x01
3G	0x02
4G	0x04
WIFI	0x10
