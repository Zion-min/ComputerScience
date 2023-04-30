-- #5-1 Defining the database tables for the COMPANY database
-- table 1
create table employee(
    Fname       varchar(15) not null,
    Minit       char,
    Lname       varchar(15) not null,
    Ssn         char(9)     not null,
    Bdate       date, 
    Address     varchar(30),
    Sex         char,
    Salary      number(10,2),
    Super_ssn   char(9),
    Dno         number      not null,
    primary key(Ssn)
);

-- table 2
create table department(
    Dname       varchar(15) not null,
    Dnumber     number      not null,
    Mgr_ssn     char(9)     not null, 
    Mgr_start_date  date,
    primary key(Dnumber),
    unique(Dname),
    foreign key(Mgr_ssn) references employee(Ssn)
);

-- table 3
create table dept_locations(
    Dnumber     number      not null,
    Dlocation   varchar(15) not null,
    primary key(Dnumber, Dlocation),
    foreign key(Dnumber) references department(Dnumber)
);

-- table 4
create table project(
    Pname       varchar(15)     not null, 
    Pnumber     number          not null,
    Plocation   varchar(15),
    Dnum        number          not null,
    primary key(Pnumber),
    unique(Pname),
    foreign key(Dnum) references department(Dnumber)
);

-- table 5
create table works_on(
    Essn        char(9)         not null,
    Pno         number          not null,
    Hours       number(3,1),  
    primary key(Essn, Pno),
    foreign key(Essn) references employee(Ssn),
    foreign key(Pno) references project(Pnumber)
);

-- table 6
create table dependent(
    Essn            char(9)         not null,
    Dependent_name  varchar(15)     not null,
    Sex             char,
    Bdate           date,
    Relationship    varchar(8),
    primary key(Essn, Dependent_name),
    foreign key(Essn) references employee(Ssn)
);

-- #5-2 Populating the database
-- EMPLOYEE
insert into employee values ('John', 'B', 'Smith', '123456789', to_date('1965-01-09','yyyy-mm-dd'), '731 Fondren, Houston, TX', 'M', 30000, '333445555', 5);
insert into employee values ('Franklin', 'T', 'Wong', '333445555', to_date('1955-12-08','yyyy-mm-dd'), '638 Voss, Houston, TX', 'M', 40000, '888665555', 5);
insert into employee values ('Alicia', 'J', 'Zelaya', '999887777', to_date('1968-01-19','yyyy-mm-dd'), '3321 Castle, Spring TX', 'F', 25000, '987654321', 4);
insert into employee values ('Jennifer', 'S', 'Wallace', '987654321', to_date('1941-06-20','yyyy-mm-dd'), '291 Berry, Bellaire, TX', 'F', 43000, '888665555', 4);
insert into employee values ('Ramesh', 'K', 'Narayan', '666884444', to_date('1962-09-15','yyyy-mm-dd'), '975 Fire Oak, Hummble, TX', 'M', 38000, '333445555', 5);
insert into employee values ('Joyce', 'A', 'English', '453453453', to_date('1972-07-31','yyyy-mm-dd'), '5631 Rice, Houston, TX', 'F', 25000, '333445555', 5);
insert into employee values ('Ahmad', 'V', 'Jabbar', '987987987', to_date('1969-03-29','yyyy-mm-dd'), '980 Dallas, Houston, TX', 'M', 25000, '987654321', 4);
insert into employee values ('James', 'E', 'Borg', '888665555', to_date('1937-11-10','yyyy-mm-dd'), '450 Stone, Houston, TX', 'M', 55000, NULL, 5);

-- DEPARTMENT
insert into department values ('Research', 5, '333445555', to_date('1988-05-22', 'yyyy-mm-dd'));
insert into department values ('Administration', 4, '987654321', to_date('1995-01-01', 'yyyy-mm-dd'));
insert into department values ('Headquarters', 1, '888665555', to_date('1981-06-19', 'yyyy-mm-dd'));

-- DEPT_LOCATIONS
insert into dept_locations values (1, 'Houston');
insert into dept_locations values (4, 'Stafford');
insert into dept_locations values (5, 'Bellaire');
insert into dept_locations values (5, 'Sugarland');
insert into dept_locations values (5, 'Houston');

-- PROJECT
insert into project values ('ProductX', 1, 'Bellaire', 5);
insert into project values ('ProductY', 2, 'Sugarland', 5);
insert into project values ('ProductZ', 3, 'Houston', 5);
insert into project values ('Computerization', 10, 'Stafford', 4);
insert into project values ('Reorganization', 20, 'Houston', 1);
insert into project values ('Newbenefits', 30, 'Stafford', 4);

-- WORKS_ON
insert into works_on values ('123456789', 1, 32.5);
insert into works_on values ('123456789', 2, 7.5);
insert into works_on values ('666884444', 3, 40.0);
insert into works_on values ('453453453', 1, 20.0);
insert into works_on values ('453453453', 2, 20.0);
insert into works_on values ('333445555', 2, 10.0);
insert into works_on values ('333445555', 3, 10.0);
insert into works_on values ('333445555', 10, 10.0);
insert into works_on values ('333445555', 20, 10.0);
insert into works_on values ('999887777', 30, 30.0);
insert into works_on values ('999887777', 10, 10.0);
insert into works_on values ('987987987', 10, 35.0);
insert into works_on values ('987987987', 30, 5.0);
insert into works_on values ('987654321', 30, 20.0);
insert into works_on values ('987654321', 20, 15.0);
insert into works_on values ('888665555', 20, NULL);

-- DEPENDENT 
insert into dependent values ('333445555', 'Alice', 'F', to_date('1986-04-05', 'yyyy-mm-dd'), 'Daughter');
insert into dependent values ('333445555', 'Theodore', 'M', to_date('1983-10-25', 'yyyy-mm-dd'), 'Son');
insert into dependent values ('333445555', 'Joy', 'F', to_date('1958-05-03', 'yyyy-mm-dd'), 'Spouse');
insert into dependent values ('987654321', 'Abner', 'M', to_date('1942-02-28', 'yyyy-mm-dd'), 'Spouse');
insert into dependent values ('123456789', 'Michael', 'M', to_date('1988-01-04', 'yyyy-mm-dd'), 'Son');
insert into dependent values ('123456789', 'Alice', 'F', to_date('1988-12-30', 'yyyy-mm-dd'), 'Daughter');
insert into dependent values ('123456789', 'Elizabeth', 'F', to_date('1967-05-05', 'yyyy-mm-dd'), 'Spouse');

-- #5-3 Database querying
-- Q1
select Fname,Lname
from employee, project
where Dno = 5 and Salary <= 30000 and Dno = Dnum and Pname = 'ProductX' order by Lname asc;
-- Q2
select e.Dno, d.Dname, e.Ssn, e.Fname
from employee e, department d
where e.Dno = d.Dnumber
and e.Address like '%, TX%' and d.Mgr_ssn = '987654321' and e.Dno = d.Dnumber;
-- Q3
select e.Lname, p.Pname, w.Hours, e.Ssn, w.Essn 
from employee e, project p, works_on w, department d 
where d.Dnumber = p.Dnum and p.Pnumber = w.Pno and w.Essn = e.Ssn
and p.Pname = 'Reorganization' and w.Hours >= 1.0
order by w.Hours desc;
-- Q4
select e.Fname, e.Lname, w.Hours
from employee e, works_on w, project p
where e.Ssn = w.Essn and w.Pno = p.Pnumber
and p.Pname like 'Product%' and w.Hours >= 10
order by w.Hours asc;
-- Q5
select e.Lname, e.Fname, dd.Sex, dd.Dependent_name, dd.relationship
from employee e, dependent dd
where e.Ssn = dd.Essn and e.Super_ssn = '333445555';

-- #5-4 Database Modification - delete
-- D1
delete from dependent 
where Essn = '123456789' and relationship = 'Spouse';
-- 1행이 삭제됨
-- D2
delete from department
where Dnumber = 5;
-- 부서번호 Dnumber를 foreign key로 연결된 테이블 employee, project가 존재하기 때문에 오류 발생.
-- D3
select * from works_on;
delete from works_on 
where Pno = 30 and Hours >= 50;
select * from works_on;
-- 삭제된 행이 없음

-- #5-5 Database modification - update
-- U1
select * from project where Dnum = 4;
update project 
set Plocation = 'Daegu'
where Dnum = 4;
select * from project where Dnum = 4;
-- U3
select * from employee where Dno = 5;
update employee
set Address = '80 Daehakro Daegu'
where Dno = 5;
select * from employee where Dno = 5;



rollback;


