//Nap_Time_Studios
#ifndef TIMER_H_
#define TIMER_H_

class napTimer
{
public:
	napTimer();
	napTimer(double time);
	virtual ~napTimer();

	void setDuration(double time);
	void start();
	void start(double time);
	void stop();
	//void resetTimeLeft();

	bool update(double timePassed);
	bool checkEnd();
	bool isActive();

protected:
	double duration = 0, timeLeft = 0;
	bool active = false;
};

#endif /* TIMER_H_ */