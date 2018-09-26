"""Make email, github_email unique

Revision ID: 39b7d45e4622
Revises: 9c0f2265bc83
Create Date: 2018-09-25 12:27:30.177733+00:00

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import postgresql


# revision identifiers, used by Alembic.
revision = '39b7d45e4622'
down_revision = '9c0f2265bc83'
branch_labels = None
depends_on = None


def upgrade():
    op.create_unique_constraint("uq_user_github_email", "user", ["github_email"])


def downgrade():
    op.drop_constraint("uq_user_github_email", "user")
