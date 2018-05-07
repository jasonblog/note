from __future__ import unicode_literals
from django.db import models

class Company(models.Model):
    stockid = models.IntegerField(db_column='StockID', primary_key=True)  # Field name made lowercase.
    abbreviation = models.CharField(db_column='Abbreviation', unique=True, max_length=10, blank=True, null=True)  # Field name made lowercase.
    url = models.CharField(db_column='URL', unique=True, max_length=128, blank=True, null=True)  # Field name made lowercase.
    employees = models.IntegerField(db_column='Employees', blank=True, null=True)  # Field name made lowercase.
    capital = models.BigIntegerField(db_column='Capital', blank=True, null=True)  # Field name made lowercase.
    industryname = models.CharField(db_column='IndustryName', max_length=16)  # Field name made lowercase.
    listeddate = models.CharField(db_column='ListedDate', max_length=45, blank=True, null=True)  # Field name made lowercase.

    class Meta:
        managed = False
        db_table = 'company'


class DjangoMigrations(models.Model):
    app = models.CharField(max_length=255)
    name = models.CharField(max_length=255)
    applied = models.DateTimeField()


    class Meta:
        managed = False
        db_table = 'django_migrations'
