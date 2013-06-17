<?PHP
    
    import java.util.ArrayList;

    function array_cartesian_product($arrays)
    {
        $result = array();
        $arrays = array_values($arrays);
        $sizeIn = sizeof($arrays);
        $size = $sizeIn > 0 ? 1 : 0;
        foreach ($arrays as $array)
        $size = $size * sizeof($array);
        for ($i = 0; $i < $size; $i ++)
        {
            $result[$i] = array();
            for ($j = 0; $j < $sizeIn; $j ++)
                array_push($result[$i], current($arrays[$j]));
            for ($j = ($sizeIn -1); $j >= 0; $j --)
            {
                if (next($arrays[$j]))
                    break;
                elseif (isset ($arrays[$j]))
                reset($arrays[$j]);
            }
        }
        return $result;
    }
    
    class JEO {
        
        public function AddTemplateFactor($vars, $template) {
            
            
            
            global $FACTORS

            $domains = array();
            $vids = array();
            
            foreach($vars as $v){
                
                array_push($domains, $v->relation->domain->toArray());
                array_push($vids, $v->relation->getVIDs($v->column, $v->value)->toArray());
                
            }
            
            $crossdomain = array_cartesian_product($domains);
            foreach($crossdomain as $Ls){
                $fandw = call_user_func_array($template, $Ls);
                $feature = $fandw[0];
                $weight  = $fandw[1];
                if($feature != 0){
                    $crossvids = array_cartesian_product($vids);
                    foreach($crossvids as $Vs){
                        $FACTORS->addTemplateFactor(new ArrayList($Vs), new ArrayList($Ls), $feature, $weight );
                    }
                }
            }
            
        }
    }
    
    $jeo = new JEO();


?>