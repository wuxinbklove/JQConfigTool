#pragma once

class ArchiveAllFonts
{
public:
	ArchiveAllFonts();
	~ArchiveAllFonts();

private:
	static ArchiveAllFonts* mInstance;
	CString m_destPath;
	void readConfig();

public:
	static ArchiveAllFonts* GetInstance();
	void start();
};
