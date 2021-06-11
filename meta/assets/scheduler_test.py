import array
import time
from time import sleep

# note: the code here may not be perfect but is just here for showing a working scheduler algo

class process_class():    
    
    def __init__(self, pid): 
        self.pid = pid 
        self.task_cpu = 0
        self.tick_in_cpu = 0
        self.tick_running = 0
        self.status = 0
        self.runned_time = 0

cpu_count = 4
average_wait = 0
average_running = 0
process_count = 5
switch_count = 0
process_with_same_time_wait = 0

cpu_process = []
process_list = []


def set_process_to_wait(pid):
    process_list[pid].status = 0
    process_list[pid].tick_running = 0
    process_list[pid].tick_in_cpu = 0
    cpu_working = process_list[pid].task_cpu
    process_list[pid].task_cpu = 0
    return cpu_working

def set_process_to_run(pid, cpu):
    cpu_process[cpu] = process_list[pid]

    process_list[pid].status = 1
    process_list[pid].task_cpu = cpu
    process_list[pid].tick_running = 0
    process_list[pid].tick_in_cpu = 0


def init():
    for i in range(0, process_count):
        process_list.append(process_class(i))
        if(len(cpu_process) < cpu_count):
            cpu_process.append(process_list[i])
            set_process_to_run(i, len(cpu_process) -1)      

def get_most_waiting_process():
    global process_with_same_time_wait
    most_waiting_count = 0
    process_with_same_time_wait = 0
    pid = 0
    for proc in process_list:
        if(proc.tick_running < most_waiting_count and proc.status == 0):
            most_waiting_count = proc.tick_running
            pid = proc.pid
    return pid

def get_most_active_process():
    most_active_count = 0
    pid = 0
    for proc in process_list:
        if(proc.tick_running > most_active_count and proc.status == 1):
            most_active_count = proc.tick_running
            pid = proc.pid

    return pid


def schedule():
    global average_running 
    global average_wait 
    global switch_count
   
    if(process_count <= cpu_count): # if we don't need to switch early return
        return
        most_waiting = get_most_waiting_process() 
    
    for i in range(0, min(cpu_count, process_count - cpu_count)):
        most_active = get_most_active_process()
        most_waiting = get_most_waiting_process() 

        switch_count += 1 ## for debuwug

        average_running += process_list[most_active].tick_in_cpu ## for debug
        
        cpu_working = set_process_to_wait(most_active)

        average_wait -= process_list[most_waiting].tick_running ## for debug too

        set_process_to_run(most_waiting, cpu_working)

    

def print_working_process():
    for cpu in cpu_process:
        ##   print("process: ", cpu.pid, "working:", cpu.tick_running, "working cpu:", cpu.tick_in_cpu)
        print('\t', cpu.pid, end='\t | ')

    print("")    



def update_process_time():
    for process in process_list:
        if( process.status == 1):
            process.tick_in_cpu += 1
            process.runned_time += 1
            process.tick_running += 1
        else:
            process.tick_in_cpu = 0
            process.tick_running -= 1

def main_loop():
    init()
    for i in range(0, 1000):
        update_process_time()
        schedule()
        print_working_process()
    
    print("for ", cpu_count, "cpu, and for", process_count, "process")

    if(switch_count != 0):
        print("average run time:", average_running / switch_count)
        print("average wait time:", average_wait / switch_count)

    for process in process_list:
        print("process", process.pid, "runned: ", process.runned_time)


main_loop()