/**
 * @author Clay Whitelytning
 * @link https://github.com/cwhitelytning/Scheduler
 * @description Supports only version 1.8.2 and higher
 */
#include <amxmodx>
#include <cellarray>
#include <scheduler>

#define PLUGIN "Time Scheduler"
#define AUTHOR "Clay Whitelytning"
#define VERSION "1.6.3"

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
new selectedTaskIndex = -1

new delay // After how many seconds to check and complete tasks? (Pointer on the CVar value)
new cleanup // Cleanup mode for completed tasks (Pointer on the CVar value)

public plugin_init() 
{
  register_plugin(PLUGIN, VERSION, AUTHOR)
  register_srvcmd("scheduler_new_task", "new_task")
  register_srvcmd("scheduler_unselect_task", "unselect_task")
  register_srvcmd("scheduler_add_command", "add_command")

  tasks = ArrayCreate(Task)
  commands = ArrayCreate(Command)

  delay = register_cvar("scheduler_delay", "1.0")
  cleanup = register_cvar("scheduler_cleanup_completed_tasks", "1")

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
    task[__duration] = getSecondsTime(splitTime(endtime, task[__format]))
    
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
 * @param integer taskid
 * @noreturn
 */
executeCommands(taskid)
{
  new size = ArraySize(commands)
  for(new index = 0; index < size; ++index) {
    new command[Command]
    ArrayGetArray(commands, index, command)

    if (command[__taskid] == taskid) {
      server_cmd(command[__srvcmd])
    }
  }
}

/**
 * Deletes all commands of the specified task ID
 * @param integer taskid
 * @noreturn
 */
removeCommands(taskid)
{
  new index = ArraySize(commands)
  while(index--) {
    new command[Command]
    ArrayGetArray(commands, index, command)

    if (command[__taskid] == taskid) {
      ArrayDeleteItem(commands, index)
    }
  }
}

/**
 * Checks tasks and submits them for execution
 */
public check_time()
{
  new taskIndex = ArraySize(tasks)
  while(taskIndex--) {
    // Protecting a task from being executed while editing    
    if (selectedTaskIndex == taskIndex) continue

    new task[Task]
    ArrayGetArray(tasks, taskIndex, task)

    if (!task[__completed]) {     
      new stringNow[TIME_FORMAT_SIZE + 1]
      get_time(task[__format], stringNow, charsmax(stringNow))
      
      new secondsNow = getSecondsTime(splitTime(stringNow, task[__format]))
      new bool:isPerform = secondsNow == task[__initial]
      if (!isPerform && task[__duration]) {
        new endSeconds = task[__initial] + task[__duration]
        if (endSeconds >= SECONDS_IN_DAY) {
          // Can the task begin?
          if (!(isPerform = secondsNow > task[__initial])) {
            // Moved on to the next day
            isPerform = SECONDS_IN_DAY + secondsNow < endSeconds
          }
        } else {
          // Task will be completed within one day
          isPerform = task[__initial] < secondsNow < endSeconds
        }
      }

      if (isPerform) {
        executeCommands(taskIndex)

        // Flag "a" - do not mark the task as completed
        if (contain(task[__flags], "a") == -1) {
          task[__completed] = true
          ArraySetArray(tasks, taskIndex, task)	
        }
      }
    }

    // Remove a completed task
    if (task[__completed] && get_pcvar_bool(cleanup)) {
      removeCommands(taskIndex)
      ArrayDeleteItem(tasks, taskIndex)
    }
  }
}