from quality_control.em import make_em_answer

# Function for gathering results after a task gets enough votes from the crowd
def gather_answer(current_task_id, model_spec):
    current_task = model_spec.task_model.objects.get(task_id=current_task_id)
    current_task.em_answer = make_em_answer(current_task, model_spec)
    current_task.save()
    current_task.group.tasks_finished += 1
    current_task.group.save()
