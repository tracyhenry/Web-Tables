from django.db import models
from basecrowd.models import AbstractCrowdTaskGroup
from basecrowd.models import AbstractCrowdTask
from basecrowd.models import AbstractCrowdWorker
from basecrowd.models import AbstractCrowdWorkerResponse

# Inherited crowd models for the interface.
# No need for special subclasses, we use the base implementations.
class CrowdTaskGroup(AbstractCrowdTaskGroup): pass

class CrowdTask(AbstractCrowdTask):
    # group
    group = models.ForeignKey(CrowdTaskGroup)

class CrowdWorker(AbstractCrowdWorker):
    # tasks
    tasks = models.ManyToManyField(CrowdTask)

class CrowdWorkerResponse(AbstractCrowdWorkerResponse):
    # Corresponding task
    task = models.ForeignKey(CrowdTask)

    # The work who made the response
    worker = models.ForeignKey(CrowdWorker)



# Model for storing requests
class Request(models.Model):

    # Path of this request
    path = models.CharField(max_length=1024)

    # Json array of this request
    post_json = models.TextField()

    # Receiving time
    recv_time = models.DateTimeField()

    def __unicode__(self):
        return self.post_json
