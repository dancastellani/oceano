/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package uff.org.br.eo.autobranch;

import br.uff.ic.oceano.util.SystemUtil;

/**
 *
 * @author marapao
 */
public class TrataAutobranch {

    public static Long autobranch(String diretorioAlterado, String pathAutobranch){
        Long result = null;

        String[] partes = diretorioAlterado.split(pathAutobranch);
      String[] partes2 = partes[1].split(SystemUtil.FILESEPARATOR);
        for (String string : partes2) {
            System.out.println(string);

        }
        result = Long.parseLong(partes2[1]);

        return result;
    }


}
