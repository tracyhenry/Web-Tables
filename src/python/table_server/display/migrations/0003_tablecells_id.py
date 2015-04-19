# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('display', '0002_fuzzymatch_yagoconcept_yagoentity_yagotype'),
    ]

    operations = [
        migrations.AddField(
            model_name='tablecells',
            name='id',
            field=models.IntegerField(null=True),
            preserve_default=True,
        ),
    ]
