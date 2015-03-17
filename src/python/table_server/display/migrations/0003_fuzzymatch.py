# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('display', '0002_yagoentity'),
    ]

    operations = [
        migrations.CreateModel(
            name='FuzzyMatch',
            fields=[
                ('row', models.IntegerField()),
                ('col', models.IntegerField()),
                ('table_id', models.IntegerField(serialize=False, primary_key=True)),
                ('entity_id', models.IntegerField()),
            ],
            options={
                'db_table': 'fuzzy_match',
            },
            bases=(models.Model,),
        ),
    ]
