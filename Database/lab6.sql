-- #6-1 Aggregates
-- 1) 
select d.Dname, count(*) as NumEmps
from employee e, department d
where e.Dno=d.Dnumber
group by d.Dname
having avg(e.Salary) >= 20000
order by numEmps desc;
-- 2)
select Dnumber, Dname, count(*) as numFemps
from department, employee
where Dno=Dnumber and Sex = 'F' and Salary>=20000
group by Dnumber, Dname
order by Dnumber;

-- #6-2 
-- 1) 
select e.Fname, e.Lname, e.Salary
from employee e
where not exists (
    (select p.Pnumber
     from project p
     where p.Dnum = 1)
     minus --차집합
    (select w.Pno
     from works_on w
     where e.Ssn = w.Essn))
order by e.Salary desc;

-- 2)
select Dnumber, Dname, Fname, Lname
from employee, department
where not exists
    (select *
     from dependent
     where Ssn = Essn)
    and exists
    (select *
     from department
     where Ssn=Mgr_ssn)
    and Dnumber = Dno;

-- #6-3 Nested queries
-- 1) 
select Fname, Lname
from employee
where Dno in
    (select e.Dno
    from employee e
    where e.Salary >= all
        (select s.Salary
        from employee s)
        );

-- 2)
select Fname, Lname
from employee
where level = 2
start with Super_ssn='888665555'
connect by prior Ssn = Super_ssn;

-- 3)
select Fname, Lname
from employee 
where Salary >= 5000 +
    (select min(Salary)
    from employee);

drop view dept_summary;

-- 6-4 View
-- 1)
create view dept_summary (D, C, Total_s, Avg_s) as
    select Dno, count(*), sum(Salary), round(avg(Salary), 1)
    from employee
    group by Dno;

-- 2)
select *
from dept_summary where D = 4;
-- output >
--  D   C   Total_s Avg_s
--1 4   3   93000   31000

-- 3)
select D, C
from dept_summary
where total_s between 50000 and 100000;
-- output >
--  D   C   
--1 1   1
--2 4   3

-- 4)
select D, Avg_s
from dept_summary
where C > (select C from dept_summary where D = 1)
order by avg_s desc;
-- output >
--  D   C   
--1 5   33250
--2 4   31000

-- 5) 
update dept_summary
set d=3
where d=1;
-- view에서 update 수행 불가 오류. base table 자체에서 수정해야 함.

-- 6)
delete from dept_summary
where c>=1;
-- view에서 delete 수행 불가 오류. base table 자체를 삭제해야 

-- #6-5 Trigger
-- trigger
create or replace
trigger salary_violation
before insert or update of Salary on employee
for each row
begin
    if (:new.Salary > 80000)
    then
        dbms_output.put_line('Old salary: '|| :old.Salary);
        dbms_output.put_line('[ALERT] New salary: '|| :new.Salary);
        dbms_output.put_line('[ALERT] New salary: '|| (:new.Salary - :old.Salary));
    end if;
end;
/
-- 
alter trigger salary_violation enable;

set serveroutput on;

update employee
set Salary = Salary*2
where Ssn = '888665555';

rollback;


