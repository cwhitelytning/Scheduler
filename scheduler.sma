#pragma semicolon 1

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
#define VERSION "1.7.0"

#define TIME_FORMAT_SIZE 9

enum _:Task {
  __format[TIME_FORMAT_SIZE + 1], // %H:%M:%S
  __initial,
  __duration,
  __flags[3]
}

enum _:Command {
  __taskid,
  __srvcmd[127]
}

new Array:tasks = Invalid_Array;
new Array:commands = Invalid_Array;
new selected_taskid = -1;

new cvar_delay; // After how many seconds to check and complete tasks? (Pointer on the CVar value)

public plugin_init() 
{
  register_plugin(PLUGIN, VERSION, AUTHOR);
  register_srvcmd("scheduler_new_task", "new_task");
  register_srvcmd("scheduler_unselect_task", "unselect_task");
  register_srvcmd("scheduler_add_command", "add_command");

  tasks = ArrayCreate(Task);
  commands = ArrayCreate(Command);
  cvar_delay = register_cvar("scheduler_delay", "1.0");

  plugin_unpause();
}

public plugin_pause() 
{
  remove_task(1);
}

public plugin_unpause() 
{
  set_task(get_pcvar_float(cvar_delay), "check_time", 1, "", 0, "b");
}

public plugin_cfg() 
{
  new config_dirpath[64];
  get_localinfo("amxx_configsdir", config_dirpath, charsmax(config_dirpath));

  new config_filepath[128];
  formatex(config_filepath, charsmax(config_dirpath), "%s/scheduler.cfg", config_dirpath);
  if (filesize(config_filepath) > 0) server_cmd("exec %s", config_filepath);
}

#if AMXX_VERSION_NUM < 183
public plugin_end() 
{
  ArrayDestroy(tasks);
  ArrayDestroy(commands);
}
#endif

/**
 * Creates a new task
 * @return integer
 */
public new_task()
{
  new argc = read_argc();
  if (argc > 3) {
    new task[Task];

    // -------------------------------------------------------
    read_argv(1, task[__flags], charsmax(task[__flags]));
    read_argv(2, task[__format], charsmax(task[__format]));
    
    // Parsing the format and the initial time
    // -------------------------------------------------------
    new initial[TIME_FORMAT_SIZE + 1];
    read_argv(3, initial, charsmax(initial));
    task[__initial] = time_duration(explode_time(initial, task[__format]));

    // Parsing duration
    // -------------------------------------------------------
    new endtime[TIME_FORMAT_SIZE + 1];
    read_argv(4, endtime, charsmax(endtime));
    task[__duration] = time_duration(explode_time(endtime, task[__format]));
    
    #if AMXX_VERSION_NUM < 183
    selected_taskid = ArrayPushArray(tasks, task) ? ArraySize(tasks) - 1 : -1;
    #else
    selected_taskid = ArrayPushArray(tasks, task);
    #endif
  }
  return PLUGIN_CONTINUE;
}

/**
 * Removes from editing mode
 * @return integer
 */
public unselect_task()
{
  selected_taskid = -1;
  return PLUGIN_CONTINUE;
}

/**
 * Adds commands to the selected task
 * @return integer
 */
public add_command()
{
  if (selected_taskid != -1 && read_argc() > 1) {
    new command[Command];

    command[__taskid] = selected_taskid;
    read_args(command[__srvcmd], charsmax(command[__srvcmd]));

    ArrayPushArray(commands, command);
  }
  return PLUGIN_CONTINUE;
}

/**
 * Reads and executes a list of commands
 * @param integer taskid
 * @noreturn
 */
execute_commands(taskid)
{
  new size = ArraySize(commands);
  for(new index = 0; index < size; ++index) {
    new command[Command];
    ArrayGetArray(commands, index, command);

    if (command[__taskid] == taskid) {
      server_cmd(command[__srvcmd]);
    }
  }
}

/**
 * Deletes all commands of the specified task ID
 * @param integer taskid
 * @noreturn
 */
remove_commands(taskid)
{
  new index = ArraySize(commands);
  while(index--) {
    new command[Command];
    ArrayGetArray(commands, index, command);

    if (command[__taskid] == taskid) {
      ArrayDeleteItem(commands, index);
    }
  }
}

/**
 * Checks tasks and submits them for execution
 */
public check_time()
{
  new index = ArraySize(tasks);
  while(index--) {
    // Protecting a task from being executed while editing    
    if (selected_taskid == index) continue;

    new task[Task];
    ArrayGetArray(tasks, index, task);

    new time_as_string[TIME_FORMAT_SIZE + 1];
    get_time(task[__format], time_as_string, charsmax(time_as_string));
    
    new duration = time_duration(explode_time(time_as_string, task[__format]));
    new is_executed = duration == task[__initial];
    if (task[__duration] && !is_executed) {
      is_executed = within_time_duration(task[__initial], duration, task[__duration]);
    }

    if (is_executed) {
      execute_commands(index);

      // Flag "a" - remove a completed task
      if (contain(task[__flags], "a") == -1) {
        remove_commands(index);
        ArrayDeleteItem(tasks, index);
      }
    }
  }
}