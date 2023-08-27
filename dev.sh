NAME=sensor_array
HERE=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

case $1 in
    --env)
        conda env remove -n $NAME
        mamba env create --no-default-packages -f $HERE/conda_env.yml
    ;;
    --run|-r)
        cd $HERE/src
        shift
        python -m $NAME --ws $HERE $@
        cd $HERE
        # PYTHONPATH=$HERE/src:$PYTHONPATH
        # python -c "from limes_x.cli import main; main()"
    ;;
    --test|-t)
        # cd $HERE/src
        # shift
        # python -m limes_x outpost -c $HERE/test/outposts/local_outpost/config.yml
        # cd $HERE

        
    ;;
    *)
        echo "bad option"
        echo $1
    ;;
esac
