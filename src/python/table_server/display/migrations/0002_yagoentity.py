# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('display', '0001_initial'),
    ]

    operations = [
        migrations.CreateModel(
            name='YagoEntity',
            fields=[
                ('id', models.IntegerField(serialize=False, primary_key=True)),
                ('value', models.CharField(max_length=255)),
            ],
            options={
                'db_table': 'yago_entity',
            },
            bases=(models.Model,),
        ),
    ]
