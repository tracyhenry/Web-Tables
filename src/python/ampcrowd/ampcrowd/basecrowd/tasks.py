from quality_control.em import make_em_answer
import json

# Function for gathering results after a task gets enough votes from the crowd
def gather_answer(current_task_id, model_spec):
    current_task = model_spec.task_model.objects.get(task_id=current_task_id)

    # get em answer
    current_task.em_answer = make_em_answer(current_task, model_spec)

    # print to file
    f = open('answers.txt', 'a')
    dict = json.loads(current_task.em_answer)
    for task in dict.keys():
        f.write(task + '\t' + str(dict[task]) + '\n')
    f.close()

    # save to db
    current_task.save()
    current_task.group.tasks_finished += 1
    current_task.group.save()
