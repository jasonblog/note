from apscheduler.schedulers.blocking import BlockingScheduler
from datetime import datetime


def job_1():
    print(datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
    print("How are you?")


def job_2():
    print(datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
    print("YES!")


def job_3():
    print("NO!")


scheduler = BlockingScheduler()
scheduler.add_job(job_1, 'interval', minutes=1)  # 每1分鐘執行一次job_1 function，會印出 當下時間以及 How are you?
scheduler.add_job(job_2, 'interval', seconds=5)  # 每5秒會執行一次 job_2 function，會印出 當下時間以及 YES!

# scheduler.add_job(job_3, 'cron', day_of_week='1-6', hour=23, minute=50)
# scheduler.add_job(job_1, 'cron', day_of_week='1-6', hour=8, minute=15)
# scheduler.add_job(job_2, 'cron', day_of_week='1-6', hour=10, minute=2)
# scheduler.add_job(job_3, 'cron', day_of_week='1-6', hour=10, minute=3)
scheduler.start()

