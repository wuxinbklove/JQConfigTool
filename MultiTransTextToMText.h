#pragma once

class MultiTransTextToMText
{
public:
	MultiTransTextToMText();
	~MultiTransTextToMText();

private:
	static MultiTransTextToMText* mInstance;
	double m_xOffset;		//x偏移数值
	double m_yOffset;		//y偏移数值
	bool m_isSetSingleLine;		//是否限定单行，若限定，则多行文本width为0.0

	void readConfig();

public:
	static MultiTransTextToMText* GetInstance();
	void start();
};

