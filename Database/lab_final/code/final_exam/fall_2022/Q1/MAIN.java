package final_exam.fall_2022.Q1;
import java.sql.*;

/****
 * Don't change anything except URL, USER_NAME, USER_PASSWD
 * 
 * @author yksuh
 *
 */
public class MAIN {
	public static final String URL = "jdbc:oracle:thin:@localhost:1521:xe"; // Oracle Address
	public static final String USER_NAME = "university"; // Oracle user ID
	public static final String USER_PASSWD = "comp322"; // Oracle user password
	public static final String FINAL_EXAM_JDBC_TABLE_NAME = "ScholarshipStatus"; // The table to be created and used

	public static void main(String[] args) {
		Connection conn = null;

		/* Connect to oracle */
		try {
			Class.forName("oracle.jdbc.driver.OracleDriver");
			System.out.println("Success!");
		} catch (ClassNotFoundException e) {
			System.err.println("error = " + e.getMessage());
			System.exit(1);
		}

		try {
			conn = DriverManager.getConnection(URL, USER_NAME, USER_PASSWD);
			conn.setAutoCommit(false);
		} catch (SQLException ex) {
			System.err.println("Cannot get a connection : " + ex.getMessage());
			System.exit(1);
		}

		/* (a) */
		System.out.println("--- (a) ---");
		/* Drop table if existing */
		drop_table(conn, FINAL_EXAM_JDBC_TABLE_NAME);
		Q1.create_scholarship_status_tbl(conn, FINAL_EXAM_JDBC_TABLE_NAME);
		print_table(conn, FINAL_EXAM_JDBC_TABLE_NAME);
		System.out.println();
		try {
			conn.commit();
		} catch (SQLException ex) {
			System.err.println("Cannot get a connection : " + ex.getMessage());
			System.exit(1);
		}

		/* (b) */
		System.out.println("--- (b) ---");
		Q1.find_first_condition(conn, FINAL_EXAM_JDBC_TABLE_NAME);
		System.out.print("-- Deleted Students --");
		System.out.println();
		print_table(conn, FINAL_EXAM_JDBC_TABLE_NAME);
		System.out.println();

		/* (c) */
		System.out.println("--- (c) ---");
		Q1.find_second_condition(conn, FINAL_EXAM_JDBC_TABLE_NAME);
		System.out.print("-- Deleted Students --");
		print_table(conn, FINAL_EXAM_JDBC_TABLE_NAME);
		System.out.println();

		/* (d) */
		System.out.println("--- (d) ---");
		Q1.find_third_condition(conn, FINAL_EXAM_JDBC_TABLE_NAME);
		System.out.print("-- Deleted Students --");
		print_table(conn, FINAL_EXAM_JDBC_TABLE_NAME);
		System.out.println();

		/* (e) */
		System.out.println("--- (e) ---");
		int tot_cred_sum = Q1.get_total_credits_sum(conn, FINAL_EXAM_JDBC_TABLE_NAME);
		System.out.println("Total Credits: " + tot_cred_sum);

		try {
			conn.close();
		} catch (SQLException ex) {
			System.err.println("Cannot close" + ex.getMessage());
			System.exit(1);
		}
	}

	/**
	 * Print table contents
	 * 
	 * @param conn                       connection object
	 * @param FINAL_EXAM_JDBC_TABLE_NAME table name
	 */
	public static void print_table(Connection conn, String FINAL_EXAM_JDBC_TABLE_NAME) {
		ResultSet rs = null;
		PreparedStatement pstmt = null;
		int tuple_number = 0;
		System.out.println();
		System.out.println("TABLE " + FINAL_EXAM_JDBC_TABLE_NAME);
		try {
			pstmt = conn.prepareStatement("select * from " + FINAL_EXAM_JDBC_TABLE_NAME + " order by id");
			rs = pstmt.executeQuery();
			System.out.println(
					"ID    \t\tNAME    \tDEPT_NAME    \t\t\tTOT_CRED    \tGRADE    \tSEC_ID    \tSEMESTER    \tYEAR    \tCREDITS");
			System.out.println(
					"--------------------------------------------------------------------------------------------------------------------------------------------------------");
			while (rs.next()) {
				tuple_number = tuple_number + 1;
				String id = rs.getString(1);
				String name = rs.getString(2);
				String dept_name = rs.getString(3);
				String tot_cred = rs.getString(4);
				String grade = rs.getString(5);
				String sec_id = rs.getString(6);
				String semester = rs.getString(7);
				String year = rs.getString(8);
				String credits = rs.getString(9);
				if (name.length() < 4 && dept_name.length() < 4)
					System.out.println(id + "    \t" + name + "    \t\t" + dept_name + "    \t\t\t\t" + tot_cred
							+ "    \t\t" + grade + "    \t\t" + sec_id + "    \t\t" + semester + "    \t" + year
							+ "    \t" + credits);
				else if (name.length() < 4 && dept_name.length() < 8)
					System.out.println(id + "    \t" + name + "    \t\t" + dept_name + "    \t\t\t" + tot_cred
							+ "    \t\t" + grade + "    \t\t" + sec_id + "    \t\t" + semester + "    \t" + year
							+ "    \t" + credits);
				else if (name.length() < 4)
					System.out.println(id + "    \t" + name + "    \t\t" + dept_name + "    \t" + tot_cred + "    \t\t"
							+ grade + "    \t\t" + sec_id + "    \t\t" + semester + "    \t" + year + "    \t"
							+ credits);
				else if (dept_name.length() < 4)
					System.out.println(id + "    \t" + name + "    \t" + dept_name + "    \t\t\t\t" + tot_cred
							+ "    \t\t" + grade + "    \t\t" + sec_id + "    \t\t" + semester + "    \t" + year
							+ "    \t" + credits);
				else if (dept_name.length() > 20)
					System.out.println(id + "    \t" + name + "    \t" + dept_name + "    \t" + tot_cred + "    \t\t"
							+ grade + "    \t\t" + sec_id + "    \t\t" + semester + "    \t" + year + "    \t"
							+ credits);
				else if (dept_name.length() >= 8)
					System.out.println(id + "    \t" + name + "    \t" + dept_name + "    \t\t" + tot_cred + "    \t\t"
							+ grade + "    \t\t" + sec_id + "    \t\t" + semester + "    \t" + year + "    \t"
							+ credits);
				else
					System.out.println(id + "    \t" + name + "    \t" + dept_name + "    \t\t\t" + tot_cred
							+ "    \t\t" + grade + "    \t\t" + sec_id + "    \t\t" + semester + "    \t" + year
							+ "    \t" + credits);

			}
			if (rs != null)
				rs.close();
			if (pstmt != null)
				pstmt.close();
			System.out.println("# of Tuples : " + tuple_number);
		} catch (SQLException e) {
			System.err.println("(Print table)sql error = " + e.getMessage());
			System.exit(1);
		}
	}

	public static void drop_table(Connection conn, String FINAL_EXAM_JDBC_TABLE_NAME) {
		Statement stmt = null;
		try {
			stmt = conn.createStatement();
			stmt.executeUpdate("DROP TABLE " + FINAL_EXAM_JDBC_TABLE_NAME);
			System.out.println("DROP TABLE " + FINAL_EXAM_JDBC_TABLE_NAME + " COMPLETED");
			if (stmt != null)
				stmt.close();
		} catch (SQLException e) {
			System.err.println("(Drop table) sql error = " + e.getMessage());
		}
	}
}
