use rest;
drop table if exists users;
drop table if exists activity;
drop table if exists user_events;

create table users (
userid serial primary key,
email varchar(255) not null,
password varchar(255) not null,
date_changed date
);

Create table activity (
activity_id int primary key,
activity varchar(50) not null
);

insert into activity values (0,'UNKNOWN'),(1,'COFFEE'),(2,'EMAIL'),(3,'SNACK'),(4,'MEETING'),(5,'CALL');

create table user_events (
eventid bigserial primary key,
userid int references users(userid),
activity_id int references activity(activity_id),
event_time timestamp
);

