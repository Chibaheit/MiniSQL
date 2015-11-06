create table student (sno char(3), sname char(3) unique, sage int, sgender char(1), primary key(sno));
create table student (sno char(3), sname char(3) unique, sage int, sgender char(1), primary key(sno));
insert into student values ('dsf', 'adf', 1, 'f');
select * from student where sno = 'dsf';
