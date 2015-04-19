# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('display', '0003_tablecells_id'),
    ]

    operations = [
        migrations.CreateModel(
            name='ColRelation',
            fields=[
                ('id', models.IntegerField(serialize=False, primary_key=True)),
                ('table_id', models.IntegerField()),
                ('col_id', models.IntegerField()),
                ('relation', models.CharField(max_length=255)),
                ('verdict', models.IntegerField()),
            ],
            options={
                'db_table': 'col_relation',
            },
            bases=(models.Model,),
        ),
    ]
