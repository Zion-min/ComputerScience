--- COMP322: Introduction to Databases (Fall 2022) --------
--- Final exam lab: University Database Schema & Data -----
-----------------------------------------------------------
CREATE TABLE univ_dept
	(dept_name		VARCHAR(30), 
	 building		VARCHAR(15), 
	 budget		        NUMERIC(12,2) check (budget > 0),
	 PRIMARY KEY (dept_name)
	);

CREATE TABLE course
	(course_id		VARCHAR(8), 
	 title			VARCHAR(50), 
	 dept_name		VARCHAR(30),
	 credits		NUMERIC(2,0) check (credits > 0),
	 PRIMARY KEY (course_id),
	 FOREIGN KEY (dept_name) REFERENCES univ_dept ON DELETE SET null
	);

CREATE TABLE instructor
	(ID			VARCHAR(5), 
	 name			VARCHAR(20) not null, 
	 dept_name		VARCHAR(30), 
	 salary			NUMERIC(8,2) check (salary > 29000),
	 PRIMARY KEY (ID),
	 FOREIGN KEY (dept_name) REFERENCES univ_dept ON DELETE SET null
	);

CREATE TABLE section
	(course_id		VARCHAR(8), 
         sec_id			VARCHAR(8),
	 semester		VARCHAR(6)
	 	check (semester in ('Spring', 'Summer', 'Fall', 'Winter')), 
	 year			NUMERIC(4,0) 
		check (year > 2019 and year < 2023), 
	 PRIMARY KEY (course_id, sec_id, semester, year),
	 FOREIGN KEY (course_id) REFERENCES course ON DELETE CASCADE
	);

CREATE TABLE teaches
	(ID			VARCHAR(5), 
	 course_id		VARCHAR(8),
	 sec_id			VARCHAR(8), 
	 semester		VARCHAR(6),
	 year			NUMERIC(4,0),
	 PRIMARY KEY (ID, course_id, sec_id, semester, year),
	 FOREIGN KEY (course_id,sec_id, semester, year) REFERENCES section ON DELETE CASCADE,
	 FOREIGN KEY (ID) REFERENCES instructor ON DELETE CASCADE
	);

CREATE TABLE student
	(ID			VARCHAR(5), 
	 name			VARCHAR(20) not null, 
	 dept_name		VARCHAR(30), 
	 tot_cred		NUMERIC(3,0) check (tot_cred >= 0),
	 PRIMARY KEY (ID),
	 FOREIGN KEY (dept_name) REFERENCES univ_dept ON DELETE SET null
	);

CREATE TABLE takes
	(ID			VARCHAR(5), 
	 course_id		VARCHAR(8),
	 sec_id			VARCHAR(8), 
	 semester		VARCHAR(6),
	 year			NUMERIC(4,0),
	 grade		        VARCHAR(2),
	 PRIMARY KEY (ID, course_id, sec_id, semester, year),
	 FOREIGN KEY (course_id,sec_id, semester, year) REFERENCES section ON DELETE CASCADE,
	 FOREIGN KEY (ID) REFERENCES student ON DELETE CASCADE
	);

CREATE TABLE prereq
	(course_id		VARCHAR(8), 
	 prereq_id		VARCHAR(8),
	 PRIMARY KEY (course_id, prereq_id),
	 FOREIGN KEY (course_id) REFERENCES course ON DELETE CASCADE,
	 FOREIGN KEY (prereq_id) REFERENCES course 
);
