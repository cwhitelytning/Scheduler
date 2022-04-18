/**
 * @author Clay Whitelytning
 * @link https://github.com/cwhitelytning/Scheduler
 * @description Supports only version 1.8.3 and higher
 */
#include <amxmodx>
#include <cellarray>
#include <scheduler>

#define PLUGIN "Time Scheduler"
#define AUTHOR "Clay Whitelytning"
#define VERSION "1.6"

#define TIME_FORMAT_SIZE 9

enum _:Task {
  __format[TIME_FORMAT_SIZE + 1], // %H:%M:%S
  __initial,
  __duration,
  __flags[3],
  bool:__completed
}

enum _:Command {
  __taskid,
  __srvcmd[127]
}

new Array:tasks
new Array:commands
new delay // After how many seconds to check and complete tasks? (Pointer on the CVar value)
new selectedTaskIndex = -1

public plugin_init() 
{
  register_plugin(PLUGIN, VERSION, AUTHOR)
  register_srvcmd("scheduler_new_task", "new_task")
  register_srvcmd("scheduler_unselect_task", "unselect_task")
  register_srvcmd("scheduler_add_command", "add_command")

  tasks = ArrayCreate(Task)
  commands = ArrayCreate(Command)
  delay = register_cvar("scheduler_delay", "1.0")

  plugin_unpause()
}

public plugin_pause() 
{
  remove_task(1)
}

public plugin_unpause() 
{
  set_task(get_pcvar_float(delay), "check_time", 1, "", 0, "b")
}

public plugin_cfg() 
{
  new configDir[64]
  get_localinfo("amxx_configsdir", configDir, charsmax(configDir))

  new configFile[128]
  formatex(configFile, charsmax(configFile), "%s/scheduler.cfg", configDir)
  if (filesize(configFile) > 0) server_cmd("exec %s", configFile)
}

#if AMXX_VERSION_NUM < 183
public plugin_end() 
{
  ArrayDestroy(tasks)
  ArrayDestroy(commands)
}
#endif

/**
 * Creates a new task
 * @return integer
 */
public new_task()
{
  new argc = read_argc()
  if (argc > 3) {
    new task[Task]

    // -------------------------------------------------------
    read_argv(1, task[__flags], charsmax(task[__flags]))
    read_argv(2, task[__format], charsmax(task[__format]))
    
    // Parsing the format and the initial time
    // -------------------------------------------------------
    new initial[TIME_FORMAT_SIZE + 1]
    read_argv(3, initial, charsmax(initial))
    task[__initial] = getSecondsTime(splitTime(initial, task[__format]))

    // Parsing duration
    // -------------------------------------------------------
    new endtime[TIME_FORMAT_SIZE + 1]
    read_argv(4, endtime, charsmax(endtime))
    task[__duration] = getSecondsTime(explodeTime(endtime))
    
    // Calculate task continuation time from its end point
    if (containi(task[__flags], "b") != -1) {
      // Task will be completed the next day
      if (task[__initial] > task[__duration]) {
        task[__duration] = SECONDS_IN_DAY - task[__initial] + task[__duration]
      } else {
        // The time range is within a day
        task[__duration] -= task[__initial]
      }
    }

    #if AMXX_VERSION_NUM < 183
    selectedTaskIndex = ArrayPushArray(tasks, task) ? ArraySize(tasks) - 1 : -1
    #else
    selectedTaskIndex = ArrayPushArray(tasks, task)
    #endif
  }
  return PLUGIN_CONTINUE
}

/**
 * Removes from editing mode
 * @return integer
 */
public unselect_task()
{
  selectedTaskIndex = -1
  return PLUGIN_CONTINUE
}

/**
 * Adds commands to the selected task
 * @return integer
 */
public add_command()
{
  if (selectedTaskIndex != -1 && read_argc() > 1) {
    new command[Command]

    command[__taskid] = selectedTaskIndex
    read_args(command[__srvcmd], charsmax(command[__srvcmd]))

    ArrayPushArray(commands, command)
  }
  return PLUGIN_CONTINUE
}

/**
 * Reads and executes a list of commands
 * @param integer id
 */
executeCommands(id)
{
  new size = ArraySize(commands)
  for(new index = 0; index < size; ++index) {
    new command[Command]
    ArrayGetArray(commands, index, command)
    if (command[__taskid] == id) {
      server_cmd(command[__srvcmd])
    }
  }
}

/**
 * Checks tasks and submits them for execution
 */
public check_time()
{
  new tasksize = ArraySize(tasks)
  for (new taskIndex = 0; taskIndex < tasksize; ++taskIndex) {    
    if (selectedTaskIndex == taskIndex) continue

    new task[Task]
    ArrayGetArray(tasks, taskIndex, task)

    if (!task[__completed]) {     
      new timenow[TIME_FORMAT_SIZE + 1] // time now
      get_time(task[__format], timenow, charsmax(timenow))
      
      new now = getSecondsTime(splitTime(timenow, task[__format]))
      new bool:execute = now == task[__initial]
      if (!execute && task[__duration]) {
        new secondsLeft = task[__initial] + task[__duration]
        if (secondsLeft >= SECONDS_IN_DAY) {
          // Can the task begin?
          execute = now > task[__initial]
          // Moved on to the next day
          if (!execute) execute = SECONDS_IN_DAY + now < secondsLeft
        } else {
          // Task will be completed within one day
          execute = task[__initial] < now < secondsLeft
        }
      }

      if (execute) {
        executeCommands(taskIndex)

        // Flag "a" - do not mark the task as completed
        if (contain(task[__flags], "a") == -1) {
          task[__completed] = true
          ArraySetArray(tasks, taskIndex, task)	
        }
      }              
    }
  }
}