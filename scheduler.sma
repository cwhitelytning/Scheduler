#pragma semicolon 1

#include <amxmodx>
#include <cellarray>
#include <scheduler>

#define AUTHOR "Clay Whitelytning"
#define PLUGIN "Scheduler"
#define VERSION "2.0.0"

#define DEFAULT_TASKID 1
#define FILENAME_SIZE 128
#define DATETIME_STRING_SIZE 31

enum _:Task {
  __format[18],       //!< %Y/%m/%d-%H:%M:%S
  __begin,            //!< datetime duration
  __end               //!< datetime duration
}

enum _:Command {
  __taskid,
  __srvcmd[127]
}

new Array:tasks = Invalid_Array;
new Array:commands = Invalid_Array;

new selected_id = -1,
    cvar_check_task_delay,
    cvar_remove_task_after_executed;

register_cvars()
{
  cvar_check_task_delay = register_cvar("scheduler_delay", "1.0");
  cvar_remove_task_after_executed = register_cvar("scheduler_remove_task_after_executed", "1");
}

register_commands()
{
  register_srvcmd("scheduler_new_task", "@srvcmd_new_task");
  register_srvcmd("scheduler_select_task", "@srvcmd_select_task");
  register_srvcmd("scheduler_unselect_task", "@srvcmd_unselect_task");
  register_srvcmd("scheduler_add_command", "@srvcmd_add_command");
  register_srvcmd("scheduler_remove_command", "@srvcmd_remove_command");
}

load_configuration()
{
  new filename[FILENAME_SIZE];
  get_localinfo("amxx_configsdir", filename, charsmax(filename));
  formatex(filename, charsmax(filename), "%s/%s", filename, "scheduler.cfg");
  server_cmd("exec %s", filename);
}

public plugin_init()
{
  register_plugin(PLUGIN, VERSION, AUTHOR);
  register_commands();
  register_cvars();

  tasks = ArrayCreate(Task);
  commands = ArrayCreate(Command);

  load_configuration();
  plugin_unpause();
}

public plugin_pause()
{
  remove_task(DEFAULT_TASKID);
}

public plugin_unpause()
{
  set_task(get_pcvar_float(cvar_check_task_delay), "@check_time", DEFAULT_TASKID, .flags = "b");
}

#if AMXX_VERSION_NUM < 183
public plugin_end() 
{
  ArrayDestroy(tasks);
  ArrayDestroy(commands);
}
#endif

#if AMXX_VERSION_NUM < 183
read_argv_int(id)
{
  new buffer[11]; // 2 147 483 647
  read_argv(id, buffer, charsmax(buffer));
  return str_to_num(buffer); // Doesn't check if error returns 0
}
#endif

@srvcmd_select_task()
{
  if (read_argc() == 2) {
    new id = read_argv_int(1);
    if (ArraySize(tasks) > id) {
      selected_id = id;
    } else
      log_amx("Error when selecting task, invalid task id %d", id);

  } else
    server_print("Please use syntax: <id>");
}

@srvcmd_unselect_task()
{
  selected_id = -1;
}

@srvcmd_new_task()
{
  new argc = read_argc();
  if (argc > 2) {
    new task[Task];
    read_argv(1, task[__format], charsmax(task[__format]));

    new begin[DATETIME_STRING_SIZE];
    read_argv(2, begin, charsmax(begin));
    task[__begin] = parse_datetime(begin, task[__format]);

    if (argc > 3) {
      new end[DATETIME_STRING_SIZE];
      read_argv(3, end, charsmax(end));
      task[__end] = parse_datetime(end, task[__format]);
    }

    #if AMXX_VERSION_NUM < 183
    selected_id = ArrayPushArray(tasks, task) ? ArraySize(tasks) - 1 : -1;
    #else
    selected_id = ArrayPushArray(tasks, task);
    #endif

  } else 
    server_print("Please use syntax: <format> <time> [<end>]");
}

@srvcmd_add_command()
{
  if (selected_id > -1) {
    if (read_argc() > 1) {
      new command[Command];

      command[__taskid] = selected_id;
      read_args(command[__srvcmd], charsmax(command[__srvcmd]));
      
      ArrayPushArray(commands, command);
    } else
      server_print("Please use syntax: <command> [<args>]");

  } else
    log_amx("Error when adding command, task not selected");
}

/**
 * Deleting a command by index.
 */
@srvcmd_remove_command()
{
  if (selected_id > -1) {
    if (read_argc() == 2) {
      new index = read_argv_int(1);
      ArrayDeleteItem(commands, index);
    } else
      server_print("Please use syntax: <id>");

  } else
    log_amx("Error when deleting command, task not selected");
}

@execute_commands(taskid)
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

@remove_all_commands(taskid)
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

@remove_task(index)
{
  if (ArraySize(tasks) > index) {
    @remove_all_commands(index);
    ArrayDeleteItem(tasks, index);    
  }
}

@remove_all_tasks()
{
  new index = ArraySize(tasks);
  while(index--) @remove_task(index);
}

@check_time()
{
  new index = ArraySize(tasks);
  while(index--) {  
    if (selected_id == index) continue; // Protecting a task from being executed while editing

    new task[Task];
    ArrayGetArray(tasks, index, task);
    
    new duration = get_now_duration_ex(task[__format]);
    if (duration == task[__begin] || (task[__end] && within_time_duration(task[__begin], duration, task[__end]))) {
      @execute_commands(index);
      if (get_pcvar_bool(cvar_remove_task_after_executed)) @remove_task(index);
    }
  }
}