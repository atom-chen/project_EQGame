using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Logic.Command
{
    public interface ICommandType
    {
        string GetBeforeCommand();

        string GetCommand();

        string GetAfterCommand();
    }
}
