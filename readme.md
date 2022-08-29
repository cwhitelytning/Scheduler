# Scheduler

* The plugin is designed to complete tasks on time.
* Tested on versions: 1.8.2, 1.8.3 and 1.9.0.

## Installation

* Unzip the archive.
* Move **scheduler.inc** to **includes** section
* Compile the plugin and move it to the **plugins** section
* Create a **scheduler.cfg** file in the **configs** section
* Register the compiled plugin in the **plugins.ini** file (move its name to the very end)

## Features

* The implementation is based on tasks that can be created using commands in the configuration file or the server console.
* It does not need to connect additional modules for data storage.
* Allows you to customize the format of the specified time by combining time components.
* Has no limits on the number of tasks created and commands added to them (except for memory).
* User-friendly source code allows you to extend plugin functionality without any difficulty.
* Deleting a task and its commands after completion.
* Able to work with transitions for the next day.
* Flexible setting of task time and duration:
* Set only the initial execution time.
* Set start and end execution.

## Commands

#### New task

```c
scheduler_new_task "format" "start time" ["duration"]
```

Creates a new job.

* Each new job goes into edit mode to add commands.
* While the task is in edit mode, it is not available for execution.
* Only one task can be in edit mode.

##### Format

Specifies which time components will be used (hours - **%H,** minutes - **%M**, seconds - **%S**).

* If multiple temporary components are used, they must be separated by a colon.
* Start time and duration must use the same number of components according to the format.

##### Start time and duration

Sets the trigger time and duration of the job.

* The time must match the given format.

#### Deselect task

```c
scheduler_unselect_task
```

Removes the selected task from edit mode.

#### Add command

```c
scheduler_add_command "option1" "option2" ...
```

Adds a new command.

* Command length is limited by the value set in the source. By default it is 127 characters.

## CVars

```c
scheduler_check_delay 1.0
```

Specifies the interval for checking and executing tasks in seconds.

* Minimum value **0.1**.

```c
scheduler_remove_task_after_executed true
```

* Delete a task after completion (**true** or **false**).

## Examples

```c
// The task will be executed every time the map changes, as long as the current time is within the time range
scheduler_new_task "%H:%M" "18:00" "08:00"

// Working example of a task
scheduler_new_task %H" "18" "08"
scheduler_add_command "mp_startmoney" "850"
scheduler_add_command "sv_restartround" "1"
scheduler_unselect_task
```

