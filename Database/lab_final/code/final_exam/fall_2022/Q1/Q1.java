package final_exam.fall_2022.Q1;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

public class Q1 {
	/* (a) */
	public static void create_scholarship_status_tbl(Connection conn, String FINAL_EXAM_JDBC_TABLE_NAME) {
		ResultSet rs = null;
		Statement stmt = null;  
		String sql = "";
		try {
			stmt = conn.createStatement();
		} catch (SQLException e) {
			e.printStackTrace();
		}
		
		sql = "DROP TABLE ScholarshipStatus";
		int res;
		try {
			res = stmt.executeUpdate(sql);
			if (res == 0) System.out.println("DROP TABLE ScholarshipStatus COMPLETED");
		} catch (SQLException e1) {
			System.out.println(e1.getMessage());
		}
		
		
		sql = "CREATE TABLE ScholarshipStatus AS select s.ID, s.name, s.dept_name, s.tot_cred, t.grade, t.sec_id, t.semester, t.year, c.credits\n"
				+ "from student s ,takes t, course c\n"
				+ "where s.ID = t.ID and t.course_id = c.course_id";
		try {
			stmt.execute(sql);
			System.out.println("Table was successfully created.");
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}

	/* (b) */
	public static void find_first_condition(Connection conn, String FINAL_EXAM_JDBC_TABLE_NAME) {
		ResultSet rs = null;
		Statement stmt = null;  
		String sql = "";
		try {
			stmt = conn.createStatement();
		} catch (SQLException e) {
			e.printStackTrace();
		}
		
		sql = "delete\n"
				+ "from scholarshipstatus\n"
				+ "where dept_name != 'Computer Science' and dept_name != 'Electrical Engineering' and dept_name != 'AI' and dept_name != 'Data Science'";
		try {
			stmt.executeUpdate(sql);
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	/* (c) */
	public static void find_second_condition(Connection conn, String FINAL_EXAM_JDBC_TABLE_NAME) {
		ResultSet rs = null;
		Statement stmt = null;  
		String sql = "";
		try {
			stmt = conn.createStatement();
		} catch (SQLException e) {
			e.printStackTrace();
		}
		
		sql = "delete\n"
				+ "from scholarshipstatus\n"
				+ "where tot_cred < 90";
		try {
			stmt.executeUpdate(sql);
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public static <K, V> K getKey(Map<K, V> map, V value) {
		 
        for (K key : map.keySet()) {
            if (value.equals(map.get(key))) {
                return key;
            }
        }
        return null;
    }
	
	/* (d) */
	public static void find_third_condition(Connection conn, String FINAL_EXAM_JDBC_TABLE_NAME) {
		ResultSet rs = null;
		Statement stmt = null;  
		String sql = "";
		try {
			stmt = conn.createStatement();
		} catch (SQLException e) {
			e.printStackTrace();
		}
		
		HashMap<String,Double> grd = new HashMap<String,Double>(){{
		    put("A+",4.3);
		    put("A0",4.0);
		    put("A-",3.7);
		    
		    put("B+",3.3);
		    put("B0",3.0);
		    put("B-",2.7);
		    
		    put("C+",2.3);
		    put("C0",2.0);
		    put("C-",1.7);
		    
		    put("D+",1.3);
		    put("D0",1.0);
		    put("D-",0.7);
		    
		    put("F",0.0);
		}};
		
		
		HashMap<String,Double> student = new HashMap<String,Double>();
		HashMap<String,Integer> crdsum = new HashMap<String,Integer>();
		HashMap<String,String> dept = new HashMap<String,String>();
		HashMap<String,String> best = new HashMap<String,String>()
				{{
					put("Computer Science", "");
					put("Electrical Engineering", "");
					put("Data Science", "");
					put("AI", "");
				}};
		
		
		
		
		sql = "select id, name, credits, grade, dept_name\n"
				+ "from scholarshipstatus;";
		try {
			rs = stmt.executeQuery(sql);
			while(rs.next()) {
				String id = rs.getString(1);
				String name = rs.getString(2);
				 
				int credit = rs.getInt(3);
				String grade = rs.getString(4);
				String dept_name = rs.getString(5);
				dept.put(name, dept_name);
				
				if (student.containsKey(name)) {
					for(Map.Entry<String, Double> entry: student.entrySet()) {
						  if(entry.getKey().equals(name)) {
							  student.put(name, student.get(name)+ (credit*grd.get(grade)));
							  crdsum.put(name, crdsum.get(name)+ credit);
						  }
						}
				}
				else {
					student.put(name, (credit*grd.get(grade)));
					crdsum.put(name, credit);
				}				
			}
			
			Iterator<Map.Entry<String, Integer>> entry =
			           	crdsum.entrySet().iterator();

		    while (entry.hasNext()) {
		        Map.Entry<String, Integer> element = entry.next();
		        student.put(element.getKey(), student.get(element.getKey())/element.getValue());
		    }
		    
		    
		    Iterator<Map.Entry<String, Double>> ent =
		           	student.entrySet().iterator();

		    while (ent.hasNext()) {
		        Map.Entry<String, Double> element = ent.next();
		        
		        
		        student.put(element.getKey(), student.get(element.getKey())/element.getValue());
		    }
		    
		    
			
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
		
	}

	/* (e) */
	public static int get_total_credits_sum(Connection conn, String FINAL_EXAM_JDBC_TABLE_NAME) {
		int sum = 0;
		return sum;
	}
}
