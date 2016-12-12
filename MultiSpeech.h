#pragma once

class MultiSpeech
{
public:
	static MultiSpeech& getInstance() {
        static MultiSpeech instance;
        return instance;
    }
	void Say(CString text); //Произнести текст
	CString  GetCurrentReader();
	void Unload();
private:
	MultiSpeech();
	MultiSpeech( const MultiSpeech&);  
    MultiSpeech& operator=( MultiSpeech& );
	~MultiSpeech();

};
