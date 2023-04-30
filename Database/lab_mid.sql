-- a.
select distinct i.name, i.dept_name
from instructor i, course c
where not exists(
    (select c.Dept_name
    from course c
    where c.Dept_name = 'Biology')
    minus
    (select c.Dept_name
    from course c, teaches t
    where c.Dept_name = i.Dept_name and c.Course_id = t.Course_id))
order by i.name;

-- b. 
select distinct t.course_id, c.title, t.year
from course c, teaches t
where c.course_id = t.course_id and t.semester = 'Fall'
group by t.course_id, c.title, t.year
order by t.course_id;

-- c.
select distinct t.year, t.semester, c.title
from teaches t, course c
where c.dept_name in
(select s.dept_name
from student s
where tot_cred <= 130) and t.course_id = c.course_id and c.title like '%d%' or c.title like '%D%'
order by t.year desc, t.semester asc, c.title asc;    

-- d. 
with g_ids as (select tt.id, count(tt.id) as a_grd
        from takes tt
        where tt.grade like 'A%'
        group by tt.id)
select distinct s.name, s.dept_name
from student s, takes t, g_ids
where s.id = t.id and g_ids.id = s.id and a_grd >= (select max(a_grd) from g_ids);


-- e.
select distinct s.name, s.tot_cred
from student s
where not exists
(select *
from takes tt
where tt.grade = 'B-' and tt.grade = 'B0' and tt.grade not like 'C%')
and exists
(select *
from takes tt
where tt.grade = 'B+' or tt.grade = 'A-' or tt.grade = 'A0' or tt.grade = 'A+')
order by tot_cred desc;


