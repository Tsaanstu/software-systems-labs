import Cell.*;
import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;
import org.omg.PortableServer.*;
import org.omg.PortableServer.POA;

import java.util.Properties;
import java.util.Map;
import java.util.HashMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

// Класс, реализующий IDL-интерфейс базовой станции
class StationServant extends StationPOA {
  
  /*
  TubeCallback tubeRef;
  String tubeNum;
  */
  Map<String, TubeCallback> tubesInCell = new HashMap<String, TubeCallback>();
  String[] words = {"bomba", "poroshenko", "navalniy"};
 
 // Метод регистрации трубки в базовой станции
  public int register ( TubeCallback objRef, String phoneNum ) {
     /*
     tubeRef = objRef;
     tubeNum = phoneNum;
     */
     tubesInCell.put( phoneNum, objRef );
     System.out.println( "Станция: зарегистрирована трубка " + phoneNum );
     System.out.println( "Станция: всего зарегистрировано: " + tubesInCell.size() );     
     return (1);
  };

  // Метод пересылки сообщения от трубки к трубке
  public int sendSMS ( String fromNum, String toNum, String message ) {
    //  System.out.println( "Станция: трубка " + fromNum + " посылает сообщение \"" + message + "\" пользователю " + toNum);

    // Здесь должен быть поиск объектной ссылки по номеру toNum
	for (int i = 0; i < words.length; i = i + 1) {
		boolean check = false;
		Pattern pattern = Pattern.compile("(\\b)" + words[i] + "(\\b)");
        Matcher matcher = pattern.matcher(message);
        while(matcher.find()) {
            //  System.out.println(matcher.group());
			System.out.println( "ТОВАРИЩИ ПОЛИЦАИ, ТУТ БАРДАК, РАЗБЕРИТЕСЬ!!! Станция: трубка " + fromNum + " посылает сообщение \"" + message + "\" пользователю " + toNum);
			check = true;
		}
		if (check == true) {
			break;
		}
	}
    tubesInCell.get( toNum ).sendSMS( fromNum, message );
    
    // tubeRef.sendSMS(fromNum, message);

    return (1);
  };

};

// Класс, реализующий сервер базовой станции
public class StationServer {

  public static void main(String args[]) {
    try{
      // Создание и инициализация ORB
      ORB orb = ORB.init(args, null);

      // Получение ссылки и активирование POAManager
      POA rootpoa = POAHelper.narrow(orb.resolve_initial_references("RootPOA"));
      rootpoa.the_POAManager().activate();

      // Создание серванта для CORBA-объекта "базовая станция" 
      StationServant servant = new StationServant();

      // Получение объектной ссылки на сервант
      org.omg.CORBA.Object ref = rootpoa.servant_to_reference(servant);
      Station sref = StationHelper.narrow(ref);
          
      org.omg.CORBA.Object objRef = orb.resolve_initial_references("NameService");
      NamingContextExt ncRef = NamingContextExtHelper.narrow(objRef);

      // Связывание объектной ссылки с именем
      String name = "BaseStation";
      NameComponent path[] = ncRef.to_name( name );
      ncRef.rebind(path, sref);

      System.out.println("Сервер готов и ждет ...");

      // Ожидание обращений от клиентов (трубок)
      orb.run();
    } 
    catch (Exception e) {
        System.err.println("Ошибка: " + e);
        e.printStackTrace(System.out);
    };
  };

};
